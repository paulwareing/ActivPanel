#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>

class QTextEdit;
class QComboBox;
class QVBoxLayout;
class QSettings;
class QLabel;
class QPushButton;
class QLineEdit;

class CommandFile;
class Command;
class Connection;
class IPCtrl;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void onSelectFile();
    void onCommandOpened(Command *);
    void onPortSettings(const QString&);
    void onConnect();
    void onConnected();
    void onDisconnect();
    void onDisconnected();
    void onConnectionError(const QString&);
    void onReceived(const QByteArray&);
    void onResult(const QString&);
    void onRefreshPortDataClicked();
    void onCommandClicked();
    void onClear();
    void onQuit();
    void onViewComSettings(bool);
    void onViewUdpSettings(bool);
    void onViewTcpSettings(bool);
    void onAutoHideActionTriggered(bool);
    void onConnectionTypeChanged(const QString&);
    void onHelp();
    void onCodes();

    void onWOL();

protected:
    void resizeEvent(QResizeEvent *event);
    void moveEvent(QMoveEvent *event);

private:
    void createMenu();
    void createConnectToolBar();
    void createComToolBar();
    void createUdpToolBar();
    void createTcpToolBar();
    void createStatusBar();
    void createCentralWidgets();
    void loadSettings();

    QAction *m_viewComSettingsAction;
    QAction *m_viewUdpSettingsAction;
    QAction *m_viewTcpSettingsAction;
    QAction *m_autoHideSettingAction;
    void autoHideSettings(const QString& connectionType);

    QToolBar *m_toolBarComSettings;
    QComboBox *m_comboBoxComPort;
    QComboBox *m_comboBoxBaudRate;
    QComboBox *m_comboBoxParity;
    QComboBox *m_comboBoxDataBits;
    QComboBox *m_comboBoxStopBits;

    void selectBaudRate(const QString&);
    void selectParity(const QString&);
    void selectDataBits(const QString&);
    void selectStopBits(const QString&);

    QToolBar *m_toolBarUdpSettings;
    QComboBox *m_localIpAddress;
    QLineEdit *m_localPort;
    IPCtrl *m_panelIpAddress;
    QLineEdit *m_panelPort;
    void selectLocalIpAddress(const QString&);

    QToolBar *m_toolBarTcpSettings;
    IPCtrl *m_panelTCPAddress;
    QLineEdit *m_panelTCPPort;

    void createCommandWidget();
    void clearCommandWidget();
    QWidget * m_commandWidget;
    QVBoxLayout * m_commandScrollLayout;
    QVBoxLayout * m_commandLayout;

    void openFile(const QFileInfo&);
    CommandFile *m_commandFile;

    QPushButton *m_btnConnect;
    QPushButton *m_btnDisconnect;

    QToolBar *m_toolBarConnection;
    QComboBox *m_comboBoxConnectionType;
    void selectConnectionType(const QString&);

    Connection *m_connection;

    QTextEdit * m_binaryLog;
    QTextEdit * m_textLog;
    QTextEdit * m_resultLog;
    QTextEdit * m_fileView;
    void log(QTextEdit *, const QString&);
    void forceCursorAtEnd(QTextEdit *);

    QLabel * m_lastOut;
    QLabel * m_lastIn;
    QLabel *m_portStatus;
    QSettings *m_settings;
};

#endif // MAINWINDOW_H
