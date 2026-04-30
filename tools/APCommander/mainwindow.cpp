
#include <QtWidgets>
#include <QtSerialPort>
#include <QTcpSocket>
#include <QInputDialog>

#include "mainwindow.h"
#include "command.h"
#include "commandfile.h"
#include "serialconnection.h"
#include "udpconnection.h"
#include "tcpconnection.h"
#include "ipctrl.h"
#include "uartmessage.h"

const QString _title = "ActivPanel Commander";
const QString _version = "v5.0";
const QString _copyright = "© 2017-2023 Promethean Ltd";
const QString _author = "Paul Wareing";

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
, m_commandFile(NULL)
, m_connection(NULL)
, m_settings(NULL)
{
    setWindowTitle(_title);
    setWindowIcon(QIcon(":/remote.png"));
    setContextMenuPolicy(Qt::NoContextMenu);

    createConnectToolBar();
    createComToolBar();
    createUdpToolBar();
    createTcpToolBar();
    createMenu();
    createCentralWidgets();
    createStatusBar();
    loadSettings();

    QString filePath = m_settings->value("file/commandFile", QVariant(QString())).toString();
    QFileInfo info(filePath);
    if (info.exists())
        openFile(info);

    onDisconnected();
}

MainWindow::~MainWindow()
{
    onDisconnect();
    delete m_commandFile;
}

void MainWindow::createMenu()
{
    QMenu *fileMenu = new QMenu("File");
    menuBar()->addMenu(fileMenu);

    QAction *openFileAction = new QAction("&Open", this);
    openFileAction->setShortcuts(QKeySequence::Open);
    openFileAction->setStatusTip(tr("Open a command file"));
    fileMenu->addAction(openFileAction);
    connect(openFileAction, SIGNAL(triggered(bool)), this, SLOT(onSelectFile()));

    fileMenu->addSeparator();

    QAction *wolAction = new QAction("&Wake On LAN", this);
    wolAction->setStatusTip(tr("Broadcast a UDP packet to wake a panel"));
    fileMenu->addAction(wolAction);
    connect(wolAction, SIGNAL(triggered(bool)), this, SLOT(onWOL()));

    fileMenu->addSeparator();

    QAction *quitAction = new QAction("&Quit", this);
    quitAction->setShortcuts(QKeySequence::Quit);
    quitAction->setStatusTip(tr("Close the application"));
    fileMenu->addAction(quitAction);
    connect(quitAction, SIGNAL(triggered(bool)), this, SLOT(onQuit()));

    QMenu *viewMenu = new QMenu("View");
    menuBar()->addMenu(viewMenu);

    m_viewComSettingsAction = m_toolBarComSettings->toggleViewAction();
    connect(m_viewComSettingsAction, SIGNAL(triggered(bool)), this, SLOT(onViewComSettings(bool)));
    viewMenu->addAction(m_viewComSettingsAction);

    m_viewUdpSettingsAction = m_toolBarUdpSettings->toggleViewAction();
    connect(m_viewUdpSettingsAction, SIGNAL(triggered(bool)), this, SLOT(onViewUdpSettings(bool)));
    viewMenu->addAction(m_viewUdpSettingsAction);

    m_viewTcpSettingsAction = m_toolBarTcpSettings->toggleViewAction();
    connect(m_viewTcpSettingsAction, SIGNAL(triggered(bool)), this, SLOT(onViewTcpSettings(bool)));
    viewMenu->addAction(m_viewTcpSettingsAction);

    viewMenu->addSeparator();

    m_autoHideSettingAction = new QAction("Auto Hide Settings", this);
    m_autoHideSettingAction->setCheckable(true);
    m_autoHideSettingAction->setChecked(true);
    connect(m_autoHideSettingAction, SIGNAL(triggered(bool)), this, SLOT(onAutoHideActionTriggered(bool)));
    viewMenu->addAction(m_autoHideSettingAction);

    QMenu *helpMenu = new QMenu("Help");
    menuBar()->addMenu(helpMenu);

    QAction *codesAction = new QAction("RS232 Codes", this);
    connect(codesAction, SIGNAL(triggered(bool)), this, SLOT(onCodes()));
    helpMenu->addAction(codesAction);

    helpMenu->addSeparator();

    QAction *aboutAction = new QAction("About", this);
    connect(aboutAction, SIGNAL(triggered(bool)), this, SLOT(onHelp()));
    helpMenu->addAction(aboutAction);   
}

void MainWindow::createConnectToolBar()
{
    m_toolBarConnection = new QToolBar("Connection");
    addToolBar(m_toolBarConnection);

    m_toolBarConnection->layout()->setSpacing(5);
    m_toolBarConnection->layout()->setContentsMargins(5, 5, 5, 5);

    QLabel *typeLabel = new QLabel("Connection");
    m_toolBarConnection->addWidget(typeLabel);

    m_comboBoxConnectionType = new QComboBox();
    m_toolBarConnection->addWidget(m_comboBoxConnectionType);
    m_comboBoxConnectionType->addItem("RS232");
    m_comboBoxConnectionType->addItem("UDP");
    m_comboBoxConnectionType->addItem("TCP/IP");
    connect(m_comboBoxConnectionType, SIGNAL(currentTextChanged(QString)), this, SLOT(onConnectionTypeChanged(QString)));

    m_btnConnect = new QPushButton();
    m_btnConnect->setToolTip("Connect");
    connect(m_btnConnect, SIGNAL(clicked()), this, SLOT(onConnect()));
    QPixmap connectionIcon(":/connect.png");
    m_btnConnect->setIcon(connectionIcon);
    m_toolBarConnection->addWidget(m_btnConnect);

    m_btnDisconnect = new QPushButton();
    m_btnDisconnect->setToolTip("Disconnect");
    connect(m_btnDisconnect, SIGNAL(clicked()), this, SLOT(onDisconnect()));
    QPixmap disconnectionIcon(":/disconnect.png");
    m_btnDisconnect->setIcon(disconnectionIcon);
    m_toolBarConnection->addWidget(m_btnDisconnect);
}

void MainWindow::createComToolBar()
{
    m_toolBarComSettings = new QToolBar("RS232 Settings");
    addToolBar(m_toolBarComSettings);

    m_toolBarComSettings->layout()->setSpacing(5);
    m_toolBarComSettings->layout()->setContentsMargins(5, 5, 5, 5);

    QLabel *comLabel = new QLabel("RS232: ");
    m_toolBarComSettings->addWidget(comLabel);

    m_comboBoxComPort = new QComboBox();
    m_toolBarComSettings->addWidget(m_comboBoxComPort);
    onRefreshPortDataClicked();

    QPushButton *btnRefreshPortList = new QPushButton();
    btnRefreshPortList->setToolTip("Refresh ports list");
    connect(btnRefreshPortList, SIGNAL(clicked()), this, SLOT(onRefreshPortDataClicked()));
    QPixmap refreshIcon(":/refresh.png");
    btnRefreshPortList->setIcon(refreshIcon);
    m_toolBarComSettings->addWidget(btnRefreshPortList);

    QLabel *lblBaud = new QLabel("Baud Rate");
    m_toolBarComSettings->addWidget(lblBaud);

    m_comboBoxBaudRate = new QComboBox();
    m_toolBarComSettings->addWidget(m_comboBoxBaudRate);
    m_comboBoxBaudRate->addItem("9600");
    m_comboBoxBaudRate->addItem("14400");
    m_comboBoxBaudRate->addItem("19200");
    m_comboBoxBaudRate->addItem("38400");
    m_comboBoxBaudRate->addItem("56000");
    m_comboBoxBaudRate->addItem("57600");
    m_comboBoxBaudRate->addItem("115200");

    QLabel *lblParity = new QLabel("Parity");
    m_toolBarComSettings->addWidget(lblParity);

    m_comboBoxParity = new QComboBox();
    m_toolBarComSettings->addWidget(m_comboBoxParity);
    m_comboBoxParity->addItem("None", QVariant(QSerialPort::NoParity));
    m_comboBoxParity->addItem("Even", QVariant(QSerialPort::EvenParity));
    m_comboBoxParity->addItem("Odd", QVariant(QSerialPort::OddParity));
    m_comboBoxParity->addItem("Space", QVariant(QSerialPort::SpaceParity));
    m_comboBoxParity->addItem("Mark", QVariant(QSerialPort::MarkParity));

    QLabel *lblDataBits = new QLabel("Data Bits");
    m_toolBarComSettings->addWidget(lblDataBits);

    m_comboBoxDataBits = new QComboBox();
    m_toolBarComSettings->addWidget(m_comboBoxDataBits);
    m_comboBoxDataBits->addItem("5", QVariant(QSerialPort::Data5));
    m_comboBoxDataBits->addItem("6", QVariant(QSerialPort::Data6));
    m_comboBoxDataBits->addItem("7", QVariant(QSerialPort::Data7));
    m_comboBoxDataBits->addItem("8", QVariant(QSerialPort::Data8));
    m_comboBoxDataBits->setCurrentIndex(3);

    QLabel *lblStopBits = new QLabel("Stop Bits");
    m_toolBarComSettings->addWidget(lblStopBits);

    m_comboBoxStopBits = new QComboBox();
    m_toolBarComSettings->addWidget(m_comboBoxStopBits);
    m_comboBoxStopBits->addItem("1", QVariant(QSerialPort::OneStop));
    m_comboBoxStopBits->addItem("1.5", QVariant(QSerialPort::OneAndHalfStop));
    m_comboBoxStopBits->addItem("2", QVariant(QSerialPort::TwoStop));
}

void MainWindow::createUdpToolBar()
{
    m_toolBarUdpSettings = new QToolBar("UDP Settings");
    addToolBar(m_toolBarUdpSettings);

    m_toolBarUdpSettings->layout()->setSpacing(5);
    m_toolBarUdpSettings->layout()->setContentsMargins(5, 5, 5, 5);

    QLabel *udpLabel = new QLabel("UDP: ");
    m_toolBarUdpSettings->addWidget(udpLabel);

    QLabel *udplocal = new QLabel("Local");
    m_toolBarUdpSettings->addWidget(udplocal);

    m_localIpAddress = new QComboBox();
    m_toolBarUdpSettings->addWidget(m_localIpAddress);
    m_localIpAddress->addItems(UdpConnection::localIpAddresses());

    m_localPort = new QLineEdit("8094");
    m_toolBarUdpSettings->addWidget(m_localPort);
    m_localPort->setFixedWidth(50);

    QLabel *udpPanel = new QLabel("Panel");
    m_toolBarUdpSettings->addWidget(udpPanel);

    m_panelIpAddress = new IPCtrl();
    m_toolBarUdpSettings->addWidget(m_panelIpAddress);

    m_panelPort = new QLineEdit("8093");
    m_toolBarUdpSettings->addWidget(m_panelPort);
    m_panelPort->setFixedWidth(50);
}

void MainWindow::createTcpToolBar()
{
    m_toolBarTcpSettings = new QToolBar("TCP Settings");
    addToolBar(m_toolBarTcpSettings);

    m_toolBarTcpSettings->layout()->setSpacing(5);
    m_toolBarTcpSettings->layout()->setContentsMargins(5, 5, 5, 5);

    QLabel *tcpLabel = new QLabel("TCP/IP: ");
    m_toolBarTcpSettings->addWidget(tcpLabel);

    QLabel *ipaddr = new QLabel("Panel IP Address");
    m_toolBarTcpSettings->addWidget(ipaddr);

    m_panelTCPAddress = new IPCtrl();
    m_toolBarTcpSettings->addWidget(m_panelTCPAddress);

    QLabel *ipport = new QLabel("Port");
    m_toolBarTcpSettings->addWidget(ipport);

    m_panelTCPPort = new QLineEdit("5000");
    m_toolBarTcpSettings->addWidget(m_panelTCPPort);
    m_panelTCPPort->setFixedWidth(50);
}

void MainWindow::createStatusBar()
{
    statusBar()->layout()->setContentsMargins(5, 5, 5, 5);
    statusBar()->layout()->setSpacing(5);
    QLabel *lblPort = new QLabel("Status: ");
    statusBar()->addWidget(lblPort);

    m_portStatus = new QLabel();
    m_portStatus->setFixedWidth(200);
    statusBar()->addWidget(m_portStatus);

    QLabel *lblLastOut = new QLabel("Last Command: ");
    statusBar()->addWidget(lblLastOut);

    m_lastOut = new QLabel();
    m_lastOut->setFixedWidth(200);
    statusBar()->addWidget(m_lastOut);

    QLabel *lblLastIn = new QLabel("Last Response: ");
    statusBar()->addWidget(lblLastIn);

    m_lastIn = new QLabel();
    statusBar()->addWidget(m_lastIn);
}

void MainWindow::createCentralWidgets()
{
    QWidget *widget = new QWidget();
    setCentralWidget(widget);

    QVBoxLayout *layout = new QVBoxLayout();
    widget->setLayout(layout);

    QHBoxLayout *lowerLayout = new QHBoxLayout();
    layout->addLayout(lowerLayout);

    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    lowerLayout->addWidget(splitter);

    QGroupBox *commandBox = new QGroupBox("Commands...");
    splitter->addWidget(commandBox);

    QVBoxLayout *vLayout = new QVBoxLayout();
    commandBox->setLayout(vLayout);

    m_commandWidget = new QWidget();
    vLayout->addWidget(m_commandWidget);

    createCommandWidget();

    QGroupBox *responseBox = new QGroupBox("Response...");
    splitter->addWidget(responseBox);

    QVBoxLayout *responseLayout = new QVBoxLayout();
    responseBox->setLayout(responseLayout);

    QTabWidget *tabResponse = new QTabWidget();
    responseLayout->addWidget(tabResponse);

    m_resultLog = new QTextEdit();
    m_resultLog->setReadOnly(true);
    tabResponse->addTab(m_resultLog, "Result");

    m_binaryLog = new QTextEdit();
    m_binaryLog->setReadOnly(true);
    tabResponse->addTab(m_binaryLog, "Binary");

    m_textLog = new QTextEdit();
    m_textLog->setReadOnly(true);
    tabResponse->addTab(m_textLog, "Ascii");

    m_fileView = new QTextEdit();
    m_fileView->setFont(QFont("courier"));
    tabResponse->addTab(m_fileView, "XML");

    QHBoxLayout *clearLayout = new QHBoxLayout();
    responseLayout->addLayout(clearLayout);

    QPushButton *btnClear = new QPushButton("Clear");
    connect(btnClear, SIGNAL(clicked()), this, SLOT(onClear()));
    clearLayout->addWidget(btnClear);

    clearLayout->addStretch();
}

void MainWindow::createCommandWidget()
{
    m_commandScrollLayout = new QVBoxLayout();
    m_commandScrollLayout->setContentsMargins(0,0,0,0);
    m_commandScrollLayout->setSpacing(0);
    m_commandWidget->setLayout(m_commandScrollLayout);

    QScrollArea *scrollArea = new QScrollArea();
    m_commandScrollLayout->addWidget(scrollArea);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget *widget = new QWidget();
    m_commandLayout = new QVBoxLayout();
    m_commandLayout->setContentsMargins(0,0,0,0);
    widget->setLayout(m_commandLayout);

    scrollArea->setWidget(widget);
    scrollArea->setWidgetResizable(true);
}

void MainWindow::clearCommandWidget()
{
    delete m_commandScrollLayout;
}

void MainWindow::onViewComSettings(bool b)
{
    m_toolBarComSettings->setVisible(b);
    m_settings->setValue("window/viewComSettings", QVariant(b));
}

void MainWindow::onViewUdpSettings(bool b)
{
    m_toolBarUdpSettings->setVisible(b);
    m_settings->setValue("window/viewUdpSettings", QVariant(b));
}

void MainWindow::onViewTcpSettings(bool b)
{
    m_toolBarTcpSettings->setVisible(b);
    m_settings->setValue("window/viewTcpSettings", QVariant(b));
}

void MainWindow::onAutoHideActionTriggered(bool autoHide)
{
    m_settings->setValue("window/autoHideSettings", QVariant(autoHide));
    if (autoHide)
        autoHideSettings(m_comboBoxConnectionType->currentText());
}

void MainWindow::onConnectionTypeChanged(const QString& currentText)
{
    if (m_autoHideSettingAction->isChecked())
        autoHideSettings(currentText);
}

void MainWindow::autoHideSettings(const QString& connectionType)
{
    if (connectionType=="UDP")
    {
        if (m_viewComSettingsAction->isChecked())
            m_viewComSettingsAction->trigger();

        if (m_viewTcpSettingsAction->isChecked())
            m_viewTcpSettingsAction->trigger();

        if (!m_viewUdpSettingsAction->isChecked())
            m_viewUdpSettingsAction->trigger();
    }
    else if (connectionType=="RS232")
    {
        if (!m_viewComSettingsAction->isChecked())
            m_viewComSettingsAction->trigger();

        if (m_viewTcpSettingsAction->isChecked())
            m_viewTcpSettingsAction->trigger();

        if (m_viewUdpSettingsAction->isChecked())
            m_viewUdpSettingsAction->trigger();
    }
    else if (connectionType=="TCP/IP")
    {
        if (m_viewComSettingsAction->isChecked())
            m_viewComSettingsAction->trigger();

        if (m_viewUdpSettingsAction->isChecked())
            m_viewUdpSettingsAction->trigger();

        if (!m_viewTcpSettingsAction->isChecked())
            m_viewTcpSettingsAction->trigger();
    }

}

void MainWindow::onQuit()
{
    QApplication::quit();
}

void MainWindow::onHelp()
{
    QString title = "About " + _title;
    QString help = _title + " " + _version + "\n" + _copyright + "\nby " + _author;
    QMessageBox::about(this, title, help);
}

void MainWindow::onCodes()
{
    QFileInfo info(QDir::currentPath() + "/docs", "ap5_codes.xlsx");
    if (info.exists())
    {
        QString s = info.absoluteFilePath();
        QDesktopServices::openUrl(s);
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if (m_settings != NULL)
        m_settings->setValue("window/geometry", QVariant(geometry()));
}

void MainWindow::moveEvent(QMoveEvent *event)
{
    QMainWindow::moveEvent(event);
    if (m_settings != NULL)
        m_settings->setValue("window/geometry", QVariant(geometry()));
}

void MainWindow::loadSettings()
{
    m_settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "Promethean", "APCommander");

    selectConnectionType(m_settings->value("connection/type", QVariant(QString("RS232"))).toString());

    selectBaudRate(m_settings->value("rs232/baudRate", QVariant(QString())).toString());
    selectParity(m_settings->value("rs232/parity", QVariant(QString())).toString());
    selectDataBits(m_settings->value("rs232/dataBits", QVariant(QString())).toString());
    selectStopBits(m_settings->value("rs232/stopBits", QVariant(QString())).toString());

    selectLocalIpAddress(m_settings->value("udp/localIpAddress", QVariant(QString())).toString());
    m_localPort->setText(m_settings->value("udp/localPort", QVariant(QString("8904"))).toString());
    m_panelIpAddress->setText(m_settings->value("udp/panelIpAddress", QVariant(QString())).toString());
    m_panelPort->setText(m_settings->value("udp/panellPort", QVariant(QString("8903"))).toString());

    m_panelTCPAddress->setText(m_settings->value("tcpip/panelipaddress", QVariant(QString())).toString());
    m_panelTCPPort->setText(m_settings->value("tcpip/panelport", QVariant(QString("5000"))).toString());

    bool viewComSettings = m_settings->value("window/viewComSettings", QVariant((bool)true)).toBool();
    m_viewComSettingsAction->setChecked(viewComSettings);
    m_toolBarComSettings->setVisible(viewComSettings);

    bool viewUdpSettings = m_settings->value("window/viewUdpSettings", QVariant((bool)true)).toBool();
    m_viewUdpSettingsAction->setChecked(viewUdpSettings);
    m_toolBarUdpSettings->setVisible(viewUdpSettings);

    bool viewTcpSettings = m_settings->value("window/viewTcpSettings", QVariant((bool)true)).toBool();
    m_viewTcpSettingsAction->setChecked(viewTcpSettings);
    m_toolBarTcpSettings->setVisible(viewTcpSettings);

    bool autoHideSettings = m_settings->value("window/autoHideSettings", QVariant((bool)true)).toBool();
    onAutoHideActionTriggered(autoHideSettings);

    QRect rect = m_settings->value("window/geometry", QRect()).toRect();
    if (!rect.isNull())
        setGeometry(rect);
}

void MainWindow::selectLocalIpAddress(const QString& ipAddress)
{
    if (!ipAddress.isEmpty())
    {
        int index = m_localIpAddress->findText(ipAddress);
        if (index > 0)
            m_localIpAddress->setCurrentIndex(index);
    }
}

void MainWindow::selectConnectionType(const QString& connectionType)
{
    if (!connectionType.isEmpty())
    {
        int index = m_comboBoxConnectionType->findText(connectionType);
        if (index > 0)
            m_comboBoxConnectionType->setCurrentIndex(index);
    }
}

void MainWindow::selectBaudRate(const QString& baudRate)
{
    if (!baudRate.isEmpty())
    {
        int index = m_comboBoxBaudRate->findText(baudRate);
        if (index > 0)
            m_comboBoxBaudRate->setCurrentIndex(index);
    }
}

void MainWindow::selectParity(const QString& parity)
{
    if (!parity.isEmpty())
    {
        int index = m_comboBoxParity->findText(parity);
        if (index > 0)
            m_comboBoxParity->setCurrentIndex(index);
    }
}

void MainWindow::selectDataBits(const QString& dataBits)
{
    if (!dataBits.isEmpty())
    {
        int index = m_comboBoxDataBits->findText(dataBits);
        if (index > 0)
            m_comboBoxDataBits->setCurrentIndex(index);
    }
}

void MainWindow::selectStopBits(const QString& stopBits)
{
    if (!stopBits.isEmpty())
    {
        int index = m_comboBoxStopBits->findText(stopBits);
        if (index > 0)
            m_comboBoxStopBits->setCurrentIndex(index);
    }
}

void MainWindow::onPortSettings(const QString& settings)
{
    QStringList s = settings.split(",");
    if (s.size() != 4)
        return;

    selectBaudRate(s.at(0));
    selectParity(s.at(1));
    selectDataBits(s.at(2));
    selectStopBits(s.at(3));
}

void MainWindow::onSelectFile()
{
    QString path = m_settings->value("file/commandFilePath", QVariant(QString())).toString();
    if (path.isEmpty())
        path = QDir::homePath();

    QString filePath = QFileDialog::getOpenFileName(this, "Open Command File", path, "*.commands");
    if (filePath.isEmpty())
        return;

    QFileInfo info(filePath);
    m_settings->setValue("file/commandFilePath", info.path());

    clearCommandWidget();
    createCommandWidget();
    setWindowTitle(_title);
    delete m_commandFile;

    openFile(info);
}

void MainWindow::openFile(const QFileInfo& info)
{
    m_commandFile = new CommandFile();
    connect(m_commandFile, SIGNAL(commandOpened(Command*)), this, SLOT(onCommandOpened(Command*)));
    connect(m_commandFile, SIGNAL(portSettings(QString)), this, SLOT(onPortSettings(QString)));

    if (m_commandFile->open(info.filePath()))
    {
        setWindowTitle(_title + " - " + info.fileName());
        m_settings->setValue("file/commandFile", info.filePath());
        m_fileView->setText(m_commandFile->text());
    }
    else
    {
        QString s = QString("Failed to open %1").arg(info.fileName());
        QMessageBox::critical(this, "AP5 Remote Commander", s);
        delete m_commandFile;
        m_commandFile = NULL;
    }

     m_commandLayout->addStretch();
}

void MainWindow::onCommandOpened(Command *command)
{
    QPushButton *btn = new QPushButton(command->name());
    btn->setToolTip(Message::toString(command->bytes()));
    connect(btn, SIGNAL(clicked()), this, SLOT(onCommandClicked()));
    m_commandLayout->addWidget(btn);
}

void MainWindow::onRefreshPortDataClicked()
{
    m_comboBoxComPort->clear();
    m_comboBoxComPort->addItems(SerialConnection::availablePorts());
}

void MainWindow::onConnect()
{
    QString settings;

    delete m_connection;
    if (m_comboBoxConnectionType->currentText()=="RS232")
    {
        m_connection = new SerialConnection();
        settings = SerialConnection::settingsToString(m_comboBoxComPort->currentText(),
                                                      m_comboBoxBaudRate->currentText().toInt(),
                                                      (QSerialPort::Parity)m_comboBoxParity->currentData().toInt(),
                                                      (QSerialPort::DataBits)m_comboBoxDataBits->currentData().toInt(),
                                                      (QSerialPort::StopBits)m_comboBoxStopBits->currentData().toInt());
    }
    else if (m_comboBoxConnectionType->currentText()=="UDP")
    {
        m_connection = new UdpConnection();
        settings = UdpConnection::settingsToString(m_localIpAddress->currentText(),
                                                   m_localPort->text().toInt(),
                                                   m_panelIpAddress->text(),
                                                   m_panelPort->text().toInt());
    }
    else if (m_comboBoxConnectionType->currentText()=="TCP/IP")
    {
        m_connection = new TcpConnection();
        settings = TcpConnection::settingsToString(m_panelTCPAddress->text(),
                                                   m_panelTCPPort->text().toInt());
    }

    connect(m_connection, SIGNAL(opened()), this, SLOT(onConnected()));
    connect(m_connection, SIGNAL(closed()), this, SLOT(onDisconnected()));
    connect(m_connection, SIGNAL(error(QString)), this, SLOT(onConnectionError(QString)));
    connect(m_connection, SIGNAL(received(QByteArray)), this, SLOT(onReceived(QByteArray)));
    connect(m_connection, SIGNAL(result(QString)), this, SLOT(onResult(QString)));

    if (!m_connection->open(settings))
    {
        delete m_connection;
        m_connection = NULL;
        return;
    }
}

void MainWindow::onConnected()
{
    m_portStatus->setText(m_connection->connectionSummary());

    m_settings->setValue("connection/type", m_comboBoxConnectionType->currentText());
    if (m_comboBoxConnectionType->currentText()=="RS232")
    {
        m_settings->setValue("rs232/baudRate", m_comboBoxBaudRate->currentText());
        m_settings->setValue("rs232/parity", m_comboBoxParity->currentText());
        m_settings->setValue("rs232/dataBits", m_comboBoxDataBits->currentText());
        m_settings->setValue("rs232/stopBits", m_comboBoxStopBits->currentText());
    }
    else if (m_comboBoxConnectionType->currentText()=="UDP")
    {
        m_settings->setValue("udp/localIpAddress", m_localIpAddress->currentText());
        m_settings->setValue("udp/localPort", m_localPort->text());
        m_settings->setValue("udp/panelIpAddress", m_panelIpAddress->text());
        m_settings->setValue("udp/panelPort", m_panelPort->text());
    }
    else if (m_comboBoxConnectionType->currentText()=="TCP/IP")
    {
        m_settings->setValue("tcpip/panelipaddress", m_panelTCPAddress->text());
        m_settings->setValue("tcpip/panelport", m_panelTCPPort->text());
    }

    m_toolBarComSettings->setEnabled(false);
    m_toolBarUdpSettings->setEnabled(false);
    m_btnConnect->setEnabled(false);
    m_btnDisconnect->setEnabled(true);
    m_comboBoxConnectionType->setEnabled(false);
}

void MainWindow::onDisconnect()
{
    if (m_connection == NULL)
        return;

    m_connection->close();
    delete m_connection;
    m_connection = NULL;
}

void MainWindow::onDisconnected()
{
    m_portStatus->setText("disconnected");
    m_toolBarComSettings->setEnabled(true);
    m_toolBarUdpSettings->setEnabled(true);
    m_btnConnect->setEnabled(true);
    m_btnDisconnect->setEnabled(false);
    m_comboBoxConnectionType->setEnabled(true);
}

void MainWindow::onConnectionError(const QString& err)
{
    m_toolBarComSettings->setEnabled(true);
    m_portStatus->setText(err);
}

void MainWindow::onReceived(const QByteArray& bytes)
{
    log(m_textLog, QString(bytes));

    QString s = Message::toString(bytes).append(" ");
    log(m_binaryLog, s + "\n");
    m_lastIn->setText(s);
}

void MainWindow::onResult(const QString& result)
{
    log(m_resultLog, result+"\n");
}

void MainWindow::onCommandClicked()
{
    if (m_connection == NULL)
        return;

    if (!m_connection->isOpen())
        return;

    QPushButton *btn = qobject_cast<QPushButton *>(this->sender());
    if (btn==NULL)
        return;

    Command *command = m_commandFile->command(btn->text());
    if (command==NULL)
        return;

    QByteArray sent;
    if (command->inputType()=="value")
    {
        QString s = QString("Enter a value between %1 and %2.").arg(command->dataMin()).arg(command->dataMax());
        bool ok = false;
        char dataByte = (char)QInputDialog::getInt(this, command->name(), s, 0, command->dataMin(), command->dataMax(), 1, &ok);
        if (!ok) return;

        sent = m_connection->send(command, dataByte);
    }
    else if (command->inputType()=="enum")
    {
        QStringList codes = command->inputEnumCodes();
        QStringList options;
        foreach(QString code, codes)
            options.append(command->inputEnumName(code));

        bool ok = false;
        QString option = QInputDialog::getItem(this, command->name(), tr("Select:"), options, 0, false, &ok);
        if (!ok || option.isEmpty()) return;

        char dataByte = (char)command->inputEnumCode(option).toInt();
        sent = m_connection->send(command, dataByte);
    }
    else if (command->inputType()=="message")
    {
        //QString s = QString("Enter a message to send");
        //bool ok = false;
        //QString msg = (QString)QInputDialog::getText(this, command->name(), tr("Select:"),QLineEdit::Normal,QString(),&ok );

        //i//f (!ok || msg.isEmpty()) return;

        UARTMessage *um = new UARTMessage();
        um->setModal(true);
        int res = um->exec();
        if( res == QDialog::Rejected )
            return;

        QString message = um->getMessage();
        QString target = um->getSource();
        Command::Endpoint ep = Command::Laptop;
        if (target == "OPS")
            ep = Command::OPS;

        sent = m_connection->send(command, message, ep);

        delete um;
    }
    else
    {
        sent = m_connection->send(command, 0);
    }

    m_lastOut->setText(Message::toString(sent));
}

void MainWindow::onClear()
{
    m_binaryLog->clear();
    m_textLog->clear();
    m_resultLog->clear();
}

void MainWindow::log(QTextEdit *textEdit, const QString& s )
{
    forceCursorAtEnd(textEdit);
    textEdit->insertPlainText( s );
    forceCursorAtEnd(textEdit);
}

void MainWindow::forceCursorAtEnd(QTextEdit *textEdit)
{
    QTextCursor cursor = textEdit->textCursor();
    cursor.setPosition( textEdit->document()->characterCount() - 1 );
    textEdit->setTextCursor( cursor );
}

void MainWindow::onWOL()
{
    QStringList addresses = (m_settings->value("WOL/MACAddresses", QVariant(QString())).toStringList());

    bool ok;
    QString s = QInputDialog::getItem(this, "Wake On LAN", "MAC Address - 'xx:xx:xx:xx:xx:xx'", addresses, 0, true, &ok);

    if (s.isEmpty())
        return;

    if (s.length() != 17)
        return;

    QStringList parts = s.split(":");
    if (parts.length() != 6 )
        return;

    char MACAddr [6];
    for (int i=0 ; i<6 ; ++i)
    {
        bool ok;
        MACAddr[i] = parts.at(i).toInt( &ok, 16);
        if (!ok)
            return;
    }

    if (!addresses.contains(s))
    {
        addresses.append(s);
        m_settings->setValue("WOL/MACAddresses", QVariant(addresses));
    }

    char MagicPacket [102]; // Magic package for remote boot

    // Set to hexadecimal before the magicpacket array 6 characters ff
    memset (MagicPacket, 0xff, 6);

    // leave the first 6 bytes as 0xff
    int packetoffset = 6;

    // Build MagicPacket.
    for (int i = 0; i <16; i++)
    {
        memcpy (MagicPacket + packetoffset, MACAddr, 6);
        packetoffset += 6;
    }

    QHostAddress FakeAddress;
    FakeAddress.setAddress ("255.255.255.255");

    QUdpSocket udpSocket;
    udpSocket.writeDatagram(MagicPacket, 102, FakeAddress, 9);
}
