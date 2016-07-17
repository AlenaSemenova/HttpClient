#include<settingswindow.h>

SettingsWindow::SettingsWindow(QWidget* pobj/*=0*/) : QWidget(pobj)
{
    quotesText = new QLabel("Quotes:");
    freqText = new QLabel("Frequency:");

    quotesEdit = new QLineEdit;
    freqEdit = new QLineEdit;
    fileCheckBox = new QCheckBox("File Record");
    goButton = new QPushButton("GO");

    Layout = new QGridLayout;
    Layout ->addWidget(quotesText, 0, 0);
    Layout ->addWidget(quotesEdit, 0, 1);
    Layout ->addWidget(freqText, 1, 0);
    Layout ->addWidget(freqEdit, 1, 1);
    Layout ->addWidget(fileCheckBox, 2, 0, 1, -1, Qt::AlignLeft);
    Layout ->addWidget(goButton, 3, 0, 1, -1);
    setLayout(Layout);

    connect(goButton, SIGNAL(clicked()), this, SLOT(settingsChanged()));

    threadSender = new QThread;
    requestSender = new RequestSender;
    requestSender -> moveToThread(threadSender);

    connect(this, SIGNAL(settingsChangedCorrectly(const QString &, int, bool)),
            requestSender, SLOT(changeSettings(const QString &, int, bool)));

    threadSender -> start();
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
    if (frequency < 400)
    {
        qDebug() << "Frequeny: Error input data. It has to be equal or greater than 400" << endl;
        return;
    }

    emit settingsChangedCorrectly(text, frequency, fileCheckBox -> isChecked());
}
