#include <QtWidgets/QApplication>
#include <QCoreApplication>
#include <QMainWindow>
#include <QFileInfo>
#include <QMessageBox>
#include <QWebEngineView>
#include <QWebEngineSettings>
#include <QUrl>
#include <QApplication>

int main(int argc, char* argv[])
{
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS",
        "--enable-gpu "
        "--ignore-gpu-blocklist "
        "--enable-zero-copy "
        "--enable-native-gpu-memory-buffers");
    QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL, false);
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);


    QApplication app(argc, argv);

    QMainWindow window;
    auto* view = new QWebEngineView(&window);
    window.setCentralWidget(view);

    view->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    view->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    view->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    view->settings()->setAttribute(QWebEngineSettings::WebGLEnabled, true);
    view->settings()->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, true);

    const QString htmlPath = QCoreApplication::applicationDirPath() + "/index.html";
    view->load(QUrl::fromLocalFile(htmlPath));

    window.resize(1400, 900);
    window.show();

    return app.exec();
}