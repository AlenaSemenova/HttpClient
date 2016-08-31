#include<settingswindow.h>

SettingsWindow::SettingsWindow(QWidget* pobj) : QWidget(pobj)
{
    quotesText = new QLabel("Quotes:", this);
    periodText = new QLabel("Time period:", this);

    quotesEdit = new QLineEdit(this);
    periodEdit = new QLineEdit(this);
    recordCheckBox = new QCheckBox("Record", this);
    goButton = new QPushButton("GO", this);
    pauseButton = new QPushButton("Pause", this);
    textEdit = new QTextEdit(this);

    Layout = new QGridLayout(this);

    Layout ->addWidget(textEdit, 0, 0, 1, -1);
    Layout ->addWidget(quotesText, 1, 0);
    Layout ->addWidget(quotesEdit, 1, 1);
    Layout ->addWidget(periodText, 2, 0);
    Layout ->addWidget(periodEdit, 2, 1);
    Layout ->addWidget(recordCheckBox, 3, 0, 1, -1, Qt::AlignLeft);
    Layout ->addWidget(goButton, 4, 0, 1, -1);
    Layout ->addWidget(pauseButton, 5, 0, 1, -1);
    setLayout(Layout);

    connect(goButton, SIGNAL(clicked()), this, SLOT(settingsChanged()));

    //Создание RequestSender и ReplyHandler  
    threadReplyHandler = new QThread(this);
    replyHandler = new ReplyHandler;
    threadSender = new QThread(this);
    requestSender = new RequestSender(0, replyHandler);

    requestSender -> moveToThread(threadSender);
    replyHandler -> moveToThread(threadReplyHandler);

    connect(this, SIGNAL(settingsChangedCorrectly(const QString &, int, bool)),
        replyHandler, SLOT(changeSettings(const QString &, int, bool)));
    connect(this, SIGNAL(settingsChangedCorrectly(const QString &, int, bool)),
        requestSender, SLOT(changeSettings(const QString &, int, bool)));
    connect(pauseButton, SIGNAL(clicked()),
         requestSender, SLOT(stopTimer()));
    connect(replyHandler, SIGNAL(sendGuiText(const QString &, bool)),
         this, SLOT(printText(const QString &, bool)));
    connect(replyHandler, SIGNAL(serverConnectionEstablished(bool)),
            this, SLOT(serverConnectionHandler(bool)));
    connect(this, SIGNAL(testServerConnection()),
            requestSender, SLOT(testServerConnection()));

    // сигналы для обработки завершения работы
    connect(replyHandler, SIGNAL(destroyed()),
             requestSender, SLOT(deleteLater()));
    connect(requestSender, SIGNAL(destroyed()),
               this, SLOT(close()));
    //

    threadSender -> start();   
    threadReplyHandler -> start();

    emit testServerConnection();
}

void SettingsWindow::closeEvent(QCloseEvent * event)
{
    static int numCalls = 0;    
    if (numCalls == 0)
    {
        textEdit -> append("Closing...");
        replyHandler -> deleteLater();
        ++numCalls;
        event -> ignore();
    }
    else
    {
        qDebug() << "end: " << QTime().currentTime();
        threadSender -> quit();
        threadReplyHandler -> quit();

        threadSender -> wait();
        threadReplyHandler -> wait();

        QMessageBox* messageBox = new QMessageBox(QMessageBox::Information, "Message Box",
            "Do you want to close application?", QMessageBox::Ok, this);
        int status = messageBox -> exec();

        if (status == QMessageBox::Ok)
            event -> accept();
    }
}

void SettingsWindow::serverConnectionHandler(bool success)
{
    this -> show();
    if (success)
    {
        textEdit -> append("Server connection: OK");
    }
    else
    {
        textEdit -> append("Server connection error");
        this -> close();
    }
}

void SettingsWindow::settingsChanged()
{
    qDebug() << "start: " << QTime().currentTime();
    //Quotes
    QString text = quotesEdit->text();
    int size = text.size(), i = 0;
    while (i < size && (text[i].isDigit() || text[i] == ','))
        ++i;

    if (!(size > 0 && i == size && text[size - 1].isDigit()))
    {
        textEdit -> append("Quotes: Error input data");
        return;
    }

    //Period
    int period = periodEdit->text().toInt();
    if (period < minPeriod)
    {
        textEdit -> append("Time period: error input data. It has to be equal or greater than " + QString().setNum(minPeriod));
        return;
    }

    textEdit -> append("Input data: OK");
    emit settingsChangedCorrectly(text, period, recordCheckBox -> isChecked());
}

void SettingsWindow::printText(const QString & text, bool exit)
{
    textEdit -> append(text);

    if (exit)
        this -> close();
}
