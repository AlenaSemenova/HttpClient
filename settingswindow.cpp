#include<settingswindow.h>

SettingsWindow::SettingsWindow(QWidget* pobj/*=0*/) : QWidget(pobj)
{
    quotesText = new QLabel("Quotes:", this);
    freqText = new QLabel("Frequency:", this);

    quotesEdit = new QLineEdit(this);
    freqEdit = new QLineEdit(this);
    fileCheckBox = new QCheckBox("File Record", this);
    goButton = new QPushButton("GO", this);
    pauseButton = new QPushButton("Pause", this);
    textEdit = new QTextEdit(this);

    Layout = new QGridLayout(this);

    Layout ->addWidget(textEdit, 0, 0, 1, -1);
    Layout ->addWidget(quotesText, 1, 0);
    Layout ->addWidget(quotesEdit, 1, 1);
    Layout ->addWidget(freqText, 2, 0);
    Layout ->addWidget(freqEdit, 2, 1);
    Layout ->addWidget(fileCheckBox, 3, 0, 1, -1, Qt::AlignLeft);
    Layout ->addWidget(goButton, 4, 0, 1, -1);
    Layout ->addWidget(pauseButton, 5, 0, 1, -1);
    setLayout(Layout);

    connect(goButton, SIGNAL(clicked()), this, SLOT(settingsChanged()));

    //Создание RequestSender и ReplyHandler  
    threadReplyHandler = new QThread;
    replyHandler = new ReplyHandler;
    threadSender = new QThread;
    requestSender = new RequestSender(0, replyHandler);

    requestSender -> moveToThread(threadSender);
    replyHandler -> moveToThread(threadReplyHandler);

    connect(this, SIGNAL(settingsChangedCorrectly(const QString &, int, bool)),
                replyHandler, SLOT(changeSettings(const QString &, int, bool)));
    connect(this, SIGNAL(settingsChangedCorrectly(const QString &, int, bool)),
            requestSender, SLOT(changeSettings(const QString &, int, bool)));

    //Запускание обоих нитей  
    threadSender -> start();   
    threadReplyHandler -> start();

}

void SettingsWindow::debug()
{
    qDebug() << "deleted Odject" << endl;
}

void SettingsWindow::closeEvent(QCloseEvent * event)
{
    requestSender -> deleteLater();
    replyHandler -> deleteLater();

    QThread::currentThread() -> sleep(3);

    threadSender -> quit();
    threadReplyHandler -> quit();

    threadSender -> wait();
    threadReplyHandler -> wait();

    delete threadSender;
    delete threadReplyHandler;

    event -> accept();
}

void SettingsWindow::settingsChanged()
{
    //Quotes
    QString text = quotesEdit->text();
    int size = text.size(), i = 0;
    while (i < size && (text[i].isDigit() || text[i] == ','))
        ++i;

    if (!(size > 0 && i == size && text[size - 1].isDigit()))
    {
        qDebug() << "Quotes: Error input data" << endl;
        return;
    }

    //Frequency
    int frequency = freqEdit->text().toInt();
    if (frequency < minFreq)
    {
        qDebug() << "Frequeny: Error input data. It has to be equal or greater than " << minFreq << endl;
        return;
    }

    emit settingsChangedCorrectly(text, frequency, fileCheckBox -> isChecked());
}
