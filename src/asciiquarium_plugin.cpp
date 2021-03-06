/*
 * Asciiquarium - Native KDE Screensaver based on the Asciiquarium program
 *    (c) Kirk Baucom <kbaucom@schizoid.com>, which you can find at
 *    http://www.robobunny.com/projects/asciiquarium/
 *
 * Ported to KDE-based desktops years ago by Maksim Orlovich <maksim@kde.org>
 * and Michael Pyne <mpyne@kde.org>.
 *
 * Copyright (c) 2003 Kirk Baucom     <kbaucom@schizoid.com>
 * Copyright (c) 2005, 2008, 2012, 2017 Michael Pyne <mpyne@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include "asciiquarium_plugin.h"

#include <QtQml>
#include <QtDebug>
#include <QQmlEngine>
#include <QQuickImageProvider>
#include <QQuickItem>
#include <QVariant>
#include <QSize>
#include <QFontDatabase>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QString>
#include <QStringList>
#include <QRgb>
#include <QColor>

#include <utility>
#include <array>
#include <random>
#include <vector>
#include <tuple>
#include <numeric>

using spriteCell = std::pair<QLatin1Char, QRgb>;
using std::size_t;
using sprite = std::vector<std::vector<spriteCell>>;

static QFont monoFont()
{
    static QFont monoFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    return monoFont;
}

static QFontMetrics monoFontMetrics()
{
    static QFontMetrics fm(monoFont());
    return fm;
}

static QPixmap pixmapFromTextSprite (const sprite &text)
{
    static const QFont textFont(monoFont());
    static const QFontMetrics fm(monoFontMetrics());

    const size_t &h = fm.lineSpacing();
    const size_t &w = fm.width(QLatin1Char('X'));

    size_t textHeight = text.size();
    size_t textWidth = std::accumulate(
            text.begin(), text.end(), size_t(0),
            [](size_t cur, const std::vector<spriteCell>& row)
            {
                return std::max(cur, row.size());
            });

    // Use QImage since we'll do a lot of repeated draw calls
    QImage pix(textWidth * w, textHeight * h, QImage::Format_ARGB32_Premultiplied);
    pix.fill(Qt::transparent);

    QPainter p;

    if(Q_UNLIKELY(!p.begin(&pix))) {
        pix.fill(Qt::red);
        return QPixmap::fromImage(pix);
    }

    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.setRenderHint(QPainter::TextAntialiasing);
    p.setFont(textFont);

    const QLatin1Char space(' ');
    const QLatin1Char question('?');

    for(size_t j = 0; j < text.size(); j++) {
        const auto &row = text[j];
        if(row.empty()) {
            continue;
        }

        size_t first;
        size_t last = row.size() - 1;
        for (first = 0; first <= last && row[first].first == space; ++first)
            ;

        for(auto i = first; i <= last; ++i) {
            if(row[i].first == question) {
                continue;
            }

            QRect r(i * w, j * h, w, h);

            p.setPen(QColor(row[i].second));
            p.fillRect(r, Qt::black);
            p.drawText(r, Qt::AlignCenter, QString(QChar(row[i].first)));
        }
    }

    p.end();

    return QPixmap::fromImage(pix);
}

// Returns appropriate color for the given character
static QRgb colorFromChar(const ushort char_code)
{
    //Colors stolen from konsole, TEWidget.cpp
    QRgb result;

    switch (char_code)
    {
        case 'R': result = 0xFF5454; break;
        case 'r': result = 0xB21818; break;
        case 'C': result = 0x54FFFF; break;
        case 'c': result = 0x18B2B2; break;
        case 'Y': result = 0xFFFF54; break;
        case 'y': result = 0xB26818; break;
        case 'G': result = 0x54FF54; break;
        case 'g': result = 0x18B218; break;
        case 'B': result = 0x5454FF; break;
        case 'b': result = 0x1818B2; break;
        case 'M': result = 0xFF54FF; break;
        case 'm': result = 0xB218B2; break;
        case 'W': result = 0xFFFFFF; break;
        case 'w': result = 0xB2B2B2; break;
        default : result = 0xFFFFFF;
    }

    return result;
}

static sprite textSpriteFromString(
        const QString &text,
        const QString &colors,
        QRgb defaultColor
        )
{
    auto rows = text.split(QLatin1Char('\n'));
    sprite result;
    result.reserve(rows.size());

    for(const auto &row : qAsConst(rows)) {
        std::vector<spriteCell> rowVec;
        rowVec.reserve(row.size());

        std::for_each(row.begin(), row.end(), [&](const QChar &ch) {
                rowVec.emplace_back(
                        QLatin1Char(
                            ch == QLatin1Char('{') ? '?' : ch.toLatin1()
                        ),
                        defaultColor);
            });
        result.emplace_back(std::move(rowVec));
    }

    // overlay mask colors
    rows = colors.split(QLatin1Char('\n'));
    if (Q_UNLIKELY(static_cast<size_t>(rows.size()) > result.size())) {
        qWarning() << "Sprite with color" << defaultColor << "has mismatched size!";
        return result;
    }

    size_t y = 0;

    for (const auto &row : qAsConst(rows)) {
        for (int pos = 0;
                pos < row.size() && static_cast<size_t>(pos) < result[y].size();
                ++pos)
        {
            const ushort charCode = row[pos].unicode();
            if (charCode != ' ') {
                result[y][pos].second = colorFromChar(charCode);
            }
        }

        ++y;
    }

    return result;
}

// See http://stackoverflow.com/a/26351760 -- this permits converting
// a compile-time array initialization to std::array with the size implicitly
// determined by the compiler.
template<typename V, typename... T>
static constexpr auto array_of(T&&... t)
    -> std::array<V, sizeof...(T)>
{
    return { { std::forward<T>(t)... } };
}

// Note that some of these sprites have {{ characters which are
// re-encoded back to be ?? to avoid trigraph issues
// Each of the eight fish sprites here are encoded in a block of 4:
//   1. right-facing fish
//   2. right-facing color mask
//   3. left-facing fish
//   4. left-facing color mask
// {{{
auto fishImages = array_of<QLatin1String>(
QLatin1String( "       \\\n"
               "     ...\\..,\n"
               "\\" "{{" "/'       \\\n"
               " >=     (  ' >\n"
               "/{{\\      / /\n"
               "    `\"'\"'/''\n" ),
QLatin1String( "       2\n"
               "     1112111\n"
               "6  11       1\n"
               " 66     7  4 5\n"
               "6  1      3 1\n"
               "    11111311\n" ),
QLatin1String( "      /\n"
               "  ,../...\n"
               " /       '\\" "{{" "/\n"
               "< '  )     =<\n"
               " \\ \\      /{{\\\n"
               "  `'\\'\"'\"'\n" ),
QLatin1String( "      2\n"
               "  1112111\n"
               " 1       11  6\n"
               "5 4  7     66\n"
               " 1 3      1  6\n"
               "  11311111\n" ),

//////////////////////////////
QLatin1String( "    \\\n"
               "\\?/--\\\n"
               ">=  (o>\n"
               "/?\\__/\n"
               "    /\n" ),
QLatin1String( "    2\n"
               "6 1111\n"
               "66  745\n"
               "6 1111\n"
               "    3\n" ),
QLatin1String( "  /\n"
               " /--\\?/\n"
               "<o)  =<\n"
               " \\__/?\\\n"
               "  \\\n" ),
QLatin1String( "  2\n"
               " 1111 6\n"
               "547  66\n"
               " 1111 6\n"
               "  3\n" ),

//////////////////////////////
QLatin1String(
"       \\:.\n"
"\\;,{{?,;\\\\\\\\\\,,\n"
"  \\\\\\\\\\;;:::::o\n"
"  ///;;::::::::<\n"
" /;`?``/////``\n" ),
QLatin1String(
"       222\n"
"666   1122211\n"
"  6661111111114\n"
"  66611111111115\n"
" 666 113333311\n" ),
QLatin1String(
"      .:/\n"
"   ,,///;,{{?,;/\n"
" o:::::::;;///\n"
">::::::::;;\\\\\\\n"
"  ''\\\\\\\\\\''?';\\\n" ),
QLatin1String(
"      222\n"
"   1122211   666\n"
" 4111111111666\n"
"51111111111666\n"
"  113333311 666\n" ),

//////////////////////////////
QLatin1String(
"  __\n"
"><_'>\n"
"   '\n" ),
QLatin1String(
"  11\n"
"61145\n"
"   3\n" ),
QLatin1String(
" __\n"
"<'_><\n"
" `\n" ),
QLatin1String(
" 11\n"
"54116\n"
" 3\n" ),

//////////////////////////////
QLatin1String(
"   ..\\,\n"
">='   ('>\n"
"  '''/''\n" ),
QLatin1String(
"   1121\n"
"661   745\n"
"  111311\n" ),
QLatin1String(
"  ,/..\n"
"<')   `=<\n"
" ``\\```\n" ),
QLatin1String(
"  1211\n"
"547   166\n"
" 113111\n" ),

//////////////////////////////
QLatin1String(
"   \\\n"
"  / \\\n"
">=_('>\n"
"  \\_/\n"
"   /\n" ),
QLatin1String(
"   2\n"
"  1 1\n"
"661745\n"
"  111\n"
"   3\n" ),
QLatin1String(
"  /\n"
" / \\\n"
"<')_=<\n"
" \\_/\n"
"  \\\n" ),
QLatin1String(
"  2\n"
" 1 1\n"
"547166\n"
" 111\n"
"  3\n" ),

//////////////////////////////
QLatin1String(
"  ,\\\n"
">=('>\n"
"  '/\n" ),
QLatin1String(
"  12\n"
"66745\n"
"  13\n" ),
QLatin1String(
" /,\n"
"<')=<\n"
" \\`\n" ),
QLatin1String(
" 21\n"
"54766\n"
" 31\n" ),

//////////////////////////////
QLatin1String(
"  __\n"
"\\/ o\\\n"
"/\\__/\n" ),
QLatin1String(
"  11\n"
"61 41\n"
"61111\n" ),
QLatin1String(
" __\n"
"/o \\/\n"
"\\__/\\\n" ),
QLatin1String(
" 11\n"
"14 16\n"
"11116\n" )

); // fishImages[]
// }}}

static_assert(
        fishImages.size() % 4 == 0,
        "fishImages must have multiple of 4 sprites!");

// sharks
auto sharks = array_of<QLatin1String>(
QLatin1String(
"                              __\n"
"                             ( `\\\n"
"  ,{{{{{{{{{{{{{{{{{{{{{{{{{{" ")   `\\\n"
";' `.{{{{{{{{{{{{{{{{{{{{{{{{" "(     `\\__\n"
" ;   `.{{{{{{{{{{{{?__..---''          `~~~~-._\n"
"  `.   `.____...--''                       (b  `--._\n"
"    >                     _.-'      .((      ._     )\n"
"  .`.-`--...__         .-'     -.___.....-(|/|/|/|/'\n"
" ;.'{{{{{{{{?`. ...----`.___.',,,_______......---'\n"
" '{{{{{{{{{{?" "'-'\n" ),
QLatin1String(
"                                                     \n"
"                                                     \n"
"                                                     \n"
"                                                     \n"
"                                                     \n"
"                                           cR        \n"
"                                                     \n"
"                                          cWWWWWWWW  \n"
"                                                     \n"
"                                                     \n" ),
QLatin1String(
"                     __\n"
"                    /' )\n"
"                  /'   ({{{{{{{{{{{{{{{{{{{{{{{{{{,\n"
"              __/'     ){{{{{{{{{{{{{{{{{{{{{{{{.' `;\n"
"      _.-~~~~'          ``---..__{{{{{{{{{{{{?.'   ;\n"
" _.--'  b)                       ``--...____.'   .'\n"
"(     _.      )).      `-._                     <\n"
" `\\|\\|\\|\\|)-.....___.-     `-.         __...--'-.'.\n"
"   `---......_______,,,`.___.'----... .'{{{{{{{{?`.;\n"
"                                     `-`{{{{{{{{{{?`\n" ),
QLatin1String(
"                                                     \n"
"                                                     \n"
"                                                     \n"
"                                                     \n"
"                                                     \n"
"        Rc                                           \n"
"                                                     \n"
"  WWWWWWWWc                                          \n"
"                                                     \n"
"                                                     \n" )
); // sharks[]

static_assert(
        sharks.size() % 4 == 0,
        "sharks[] must have multiple of 4 sprites!");

// castle
auto castle = array_of<QLatin1String>(
QLatin1String(
"               T~~\n"
"               |\n"
"              /^\\\n"
"             /   \\\n"
" _   _   _  /     \\  _   _   _\n"
"[ ]_[ ]_[ ]/ _   _ \\[ ]_[ ]_[ ]\n"
"|_=__-_ =_|_[ ]_[ ]_|_=-___-__|\n"
" | _- =  | =_ = _    |= _=   |\n"
" |= -[]  |- = _ =    |_-=_[] |\n"
" | =_    |= - ___    | =_ =  |\n"
" |=  []- |-  /| |\\   |=_ =[] |\n"
" |- =_   | =| | | |  |- = -  |\n"
" |_______|__|_|_|_|__|_______|\n" ),
QLatin1String(
"                RR\n"
"\n"
"              yyy\n"
"             y   y\n"
"            y     y\n"
"           y       y\n"
"\n"
"\n"
"\n"
"              yyy\n"
"             yy yy\n"
"            y y y y\n"
"            yyyyyyy\n" )
); // castle[]


// oceans
auto oceanSpriteText =
    QLatin1String( "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                   "^^^^ ^^^  ^^^   ^^^    ^^^^      \n"
                   "^^^^      ^^^^     ^^^    ^^     \n"
                   "^^      ^^^^      ^^^    ^^^^^^  \n" );

class ColorImageProvider : public QQuickImageProvider
{
public:
    ColorImageProvider()
        : QQuickImageProvider(QQuickImageProvider::Pixmap)
    {
    }

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override
    {
        QPixmap result;

        if (id == QLatin1String("black")) {
            result = QPixmap(requestedSize);
            result.fill(Qt::black);
            return result;
        }

        const sprite textSprite = (id == QLatin1String("ocean"))
            ? textSpriteFromString(oceanSpriteText, QLatin1String(), 0x149494)
            : (id == QLatin1String("castle"))
            ? textSpriteFromString(castle[0], castle[1], 0x686868)
            : textSpriteFromFacingId(id);

        result = pixmapFromTextSprite(textSprite);
        if (size) {
            *size = result.size();
        }

        return result;
    }

private:
    QString colorsFromMask(QString mask)
    {
        static auto colors = array_of<char>(
            'c','C','r','R','y','Y','b','B','g','G','m','M'
        );
        static std::uniform_int_distribution<size_t> color_rng(0, colors.size() - 1);

        for (char i = '1'; i <= '9'; ++i)
        {
            // '4' is the eye pixel
            const char colorId = i == '4' ? 'W'
                                          : colors[color_rng(m_gen)];
            mask.replace(QLatin1Char(i), QLatin1Char(colorId));
        }

        return mask;
    }

    sprite textSpriteFromFacingId(const QString &id)
    {
        // Not something simple, determine direction first
        QStringList idComponents = id.split(QLatin1Char('/'));
        bool fromLeft = !idComponents.isEmpty() &&
            idComponents[0] == QLatin1String("from_left");
        if (idComponents.size() < 2 ||
                (!fromLeft && idComponents[0] != QLatin1String("from_right")))
        {
            qWarning() << "Unknown pixmap id " << id << " for asciiquarium";
            return sprite{};
        }

        size_t arraySize = 0;
        const QLatin1String *firstSprite = nullptr;
        bool randomizeColors = false;
        QRgb defaultColor = qRgb(0, 0, 0);

        auto arraySelector = [&](const auto &chosenArray) {
            arraySize = chosenArray.size();
            firstSprite = &chosenArray.front();
        };

        if (idComponents[1] == QLatin1String("shark")) {
            arraySelector(sharks);
            defaultColor = 0x18B2B2;
        } else {
            arraySelector(fishImages);
            randomizeColors = true;
        }

        std::uniform_int_distribution<unsigned> rng(0, (arraySize / 4) - 1);
        unsigned spriteId = rng(m_gen) * 4;
        spriteId += fromLeft ? 0 : 2;

        const QLatin1String &colorMask = firstSprite[spriteId + 1];
        return textSpriteFromString(
                firstSprite[spriteId],
                randomizeColors
                    ? colorsFromMask(colorMask)
                    : colorMask,
                defaultColor);
    }

private:
    std::random_device m_rd;
    std::mt19937 m_gen = std::mt19937(m_rd());
};

AsciiquariumAnimator::AsciiquariumAnimator(QObject *parent)
    : QObject(parent)
{
    // Timer updated by m_msPerStep
    QObject::connect(
            &m_timer, &QTimer::timeout,
            this, &AsciiquariumAnimator::updateProperty);
}

void AsciiquariumAnimator::setMsPerStep(int newMsPerStep)
{
    auto oldStep = m_msPerStep;

    m_msPerStep = newMsPerStep;
    if (oldStep != newMsPerStep) {
        m_timer.start(newMsPerStep);
    }
}

void AsciiquariumAnimator::updateProperty()
{
    const int sign = m_leftFacing ? -1 : 1;
    int newX = m_targetProperty.read().toInt() + sign * m_moveStep;
    QQuickItem *item = qobject_cast<QQuickItem*>(m_targetProperty.object());
    if (item && item->parent() && (newX >= item->parentItem()->width() || newX <= 0)) {
        newX = m_leftFacing ? item->parentItem()->width() : 0;
    }
    m_targetProperty.write(newX);
}

void AsciiquariumPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("org.kde.plasma.asciiquarium"));

    qmlRegisterType<AsciiquariumAnimator>(uri, 1, 0, "AsciiquariumAnimator");
}

void AsciiquariumPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    qInfo() << "Adding asciiquarium image provider to existing QML engine";

    engine->addImageProvider(
            QLatin1String("org.kde.plasma.asciiquarium"),
            new ColorImageProvider // Qt now owns the pointer
            );

    QQmlContext *qmlContext = engine->rootContext();
    const QFontMetrics &fm(monoFontMetrics());

    qmlContext->setContextProperty(
            QLatin1String("asciiquariumCellWidth"),
            QVariant(fm.width(QLatin1Char('X'))));
    qmlContext->setContextProperty(
            QLatin1String("asciiquariumCellHeight"),
            QVariant(fm.lineSpacing()));

    Q_UNUSED(uri);
}
