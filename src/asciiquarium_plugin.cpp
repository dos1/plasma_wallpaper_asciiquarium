#include "asciiquarium_plugin.h"

#include <QtQml>
#include <QtDebug>
#include <QQmlEngine>
#include <QQuickImageProvider>
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

static QPixmap pixmapFromTextSprite (
        const sprite &text,
        size_t maxTextWidth,
        size_t cellWidth,
        size_t cellHeight
        )
{
    size_t &h = cellHeight;
    size_t &w = cellWidth;
    size_t textHeight = text.size();
    size_t textWidth  = maxTextWidth;

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
    p.setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    QFontMetrics fm(p.fontMetrics());

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

static sprite textSpriteFromString(
        const QString &text,
        const QString &mask,
        QRgb defaultColor
        )
{
    auto rows = text.split(QLatin1Char('\n'));
    sprite result;
    result.reserve(rows.size());

    for(const auto &row : rows) {
        std::vector<spriteCell> rowVec;
        rowVec.reserve(row.size());

        std::for_each(row.begin(), row.end(), [&](const QChar &ch) {
                rowVec.emplace_back(
                        QLatin1Char(
                            ch == QLatin1Char('{') ? '?' : ch.toLatin1()
                        ),
                        defaultColor);
            });
        result.emplace_back(rowVec);
    }

    // overlay mask colors
    rows = mask.split(QLatin1Char('\n'));
    size_t y = 0;

    for(const auto &row : rows) {
        if (y >= result.size())
            break;

        for (int pos = 0;
                pos < row.size() && static_cast<size_t>(pos) < result[y].size();
                ++pos)
        {
            switch (row[pos].unicode())
            {
                //Colors stolen from konsole, TEWidget.cpp
                case 'R':
                    result[y][pos].second = 0xFF5454;
                    break;
                case 'r':
                    result[y][pos].second = 0xB21818;
                    break;
                case 'C':
                    result[y][pos].second = 0x54FFFF;
                    break;
                case 'c':
                    result[y][pos].second = 0x18B2B2;
                    break;
                case 'Y':
                    result[y][pos].second = 0xFFFF54;
                    break;
                case 'y':
                    result[y][pos].second = 0xB26818;
                    break;
                case 'G':
                    result[y][pos].second = 0x54FF54;
                    break;
                case 'g':
                    result[y][pos].second = 0x18B218;
                    break;
                case 'B':
                    result[y][pos].second = 0x5454FF;
                    break;
                case 'b':
                    result[y][pos].second = 0x1818B2;
                    break;
                case 'M':
                    result[y][pos].second = 0xFF54FF;
                    break;
                case 'm':
                    result[y][pos].second = 0xB218B2;
                    break;
                case 'W':
                    result[y][pos].second = 0xFFFFFF;
                    break;
                case 'w':
                    result[y][pos].second = 0xB2B2B2;
                    break;
                case ' ':
                    break;
                default:
                    result[y][pos].second = 0xFFFFFF;
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

static_assert(
        fishImages.size() % 4 == 0,
        "fishImages must have multiple of 4 sprites!");

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
        }
        else {
            std::uniform_int_distribution<unsigned> rng(0, fishImages.size() / 4 - 1);
            unsigned fish = rng(m_gen) * 4;
            fish += m_gen() % 2 ? 2 : 0; // Select left or right-facing

            const sprite fishSprite = textSpriteFromString(
                    fishImages[fish], fishImages[fish + 1], qRgb(0, 0, 0));
            size_t maxTextWidth = std::accumulate(
                    fishSprite.begin(), fishSprite.end(), size_t(0),
                    [](size_t cur, const std::vector<spriteCell>& r)
                    {
                        return std::max(cur, r.size());
                    });
            result = pixmapFromTextSprite(fishSprite, maxTextWidth,
                    14, 28);
        }

        if(size) {
            *size = result.size();
        }

        return result;
    }

private:
    std::random_device m_rd;
    std::mt19937 m_gen = std::mt19937(m_rd());
};

AsciiquariumPlugin::AsciiquariumPlugin(QObject *parent, const QVariantList &args)
: Plasma::Applet(parent, args)
{
    qInfo() << "Asciiquarium image provider applet loaded";

    // Add our image provider to the QmlEngine
    auto engine = qmlEngine(this);
    if (!engine) {
        return;
    }

    qInfo() << "Adding asciiquarium image provider to existing QML engine";

    // Qt now owns the pointer
    engine->addImageProvider(
            QLatin1String("org.kde.plasma.asciiquarium"),
            new ColorImageProvider
            );
}

AsciiquariumPlugin::~AsciiquariumPlugin()
{
}

void AsciiquariumPlugin::init()
{
    Plasma::Applet::init();
}

K_EXPORT_PLASMA_APPLET_WITH_JSON(asciiquarium_plugin, AsciiquariumPlugin, "metadata.json")

#include "asciiquarium_plugin.moc"