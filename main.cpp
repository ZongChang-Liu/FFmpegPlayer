#include <QApplication>
#include <QTranslator>
#include <QDateTime>
#include <QProcess>
#include <QSharedMemory>
#include <QDir>
#include <QMessageBox>
#include <QLockFile>
#include <QScreen>

#include "MyMainWindows.h"
#include "ElaApplication.h"
#include "FileTools/Microscope_Utils_Config.h"
#include "FileTools/Microscope_Utils_Log.h"

int main(int argc, char* argv[])
{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#else
    //根据实际屏幕缩放比例更改
    qputenv("QT_SCALE_FACTOR", "1.5");
#endif
#endif
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling); //禁用系统dpi缩放
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    qputenv("QT_SCALE_FACTOR","1"); //设置缩放因子为1，表示不根据系统dip缩放，根据屏幕分辨率自动缩放1:1

    QApplication a(argc, argv);
    QLockFile lockFile(QDir::temp().absoluteFilePath("SingleApp.lock"));
    const bool is_locked = lockFile.isLocked();
    if (!lockFile.tryLock(500))
    {
        QMessageBox::warning(nullptr, QObject::tr("警告"), QObject::tr("程序正在运行！"));
        return 0;
    }

    eApp->init();


    const QString current_date = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
    logApp->init(current_date.toStdString());
    configApp->init();
    std::string translator_config;
    if (configApp->getTranslator(translator_config) != 0)
    {
        if (QLocale::system().name() == "zh_CN")
        {
            configApp->setTranslator("zh_CN");
            translator_config = "zh_CN";
        }
        else
        {
            configApp->setTranslator("en");
            translator_config = "en";
        }
    }

    LOG_INFO("current translator_config: {}",translator_config);

    QTranslator translator;
    if (translator.load(":/resources/translations/en.qm"))
    {
        LOG_INFO("load translator");
    } else
    {
        LOG_ERROR("load translator failed");
    }


    if (translator_config == "en")
    {
        QCoreApplication::installTranslator(&translator);
    }

    const QRect geometry = QGuiApplication::primaryScreen()->availableGeometry();
    const int width = geometry.width();
    const int height = geometry.height();
    constexpr double scale = 0.75;

    MyMainWindows w;
    w.resize(static_cast<int>(width * scale),static_cast<int>(height * scale));
    w.moveToCenter();
    w.activateWindow();
    w.setWindowState((w.windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    w.showNormal();

    return QApplication::exec();
}
