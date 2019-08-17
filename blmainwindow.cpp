#include "blmainwindow.h"
#include <QDebug>

BLMainWindow::BLMainWindow(QWidget *parent)
    : QDialog(parent)
{
    this->setWindowTitle(tr("青年大学习"));
    this->resize(400,125);
    //this->setFixedHeight(125);
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);//去掉窗体的 ？ 。
    this->setWindowFlags(Qt::FramelessWindowHint |
                             Qt::WindowSystemMenuHint |
                             Qt::WindowMinMaxButtonsHint);
    createWidgets();
    createConnects();
    createStyles();
}

BLMainWindow::~BLMainWindow()
{

}

void BLMainWindow::createWidgets()
{
    seedButton = new QPushButton;
    seedPath = new QLineEdit;
    nameListButton = new QPushButton;
    nameListEdit = new QLineEdit;
    startButton = new QPushButton;
    exitButton = new QPushButton;

    mainLayout = new QVBoxLayout;
    formLayout = new QFormLayout;
    controlLyaout = new QHBoxLayout;
    seedLyaout = new QHBoxLayout;
    nameListLyaout = new QHBoxLayout;

    seedPathStr = "";
    seedImage = new QImage;
    seedLoadFlag = false;
    nameListPathStr = "";

    savePath = "";

    thread = new FileGenerate(this);
}

void BLMainWindow::createStyles()
{
    seedButton->setText(tr("源文件"));
    seedPath->setEnabled(false);
    seedLyaout->addWidget(seedButton);
    seedLyaout->addWidget(seedPath);
    formLayout->addRow(seedLyaout);
    nameListButton->setText(tr("名单"));
    nameListButton->setToolTip(tr("从文件获取名单并覆盖当前名单列表里的名单,文件中一行可以有多个姓名,姓名不可跨行"));
    nameListEdit->setEnabled(true);
    nameListEdit->setToolTip(tr("每个姓名之间以 / 隔开"));
    nameListLyaout->addWidget(nameListButton);
    nameListLyaout->addWidget(nameListEdit);
    formLayout->addRow(nameListLyaout);
    startButton->setText(tr("开始"));
    exitButton->setText(tr("退出"));
    controlLyaout->addWidget(startButton);
    controlLyaout->addWidget(exitButton);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(controlLyaout);
    //mainLayout->setSpacing(0);
    //mainLayout->addStretch();
    this->setLayout(mainLayout);
}

void BLMainWindow::createConnects()
{
    connect(seedButton,SIGNAL(clicked(bool)),this,SLOT(getSeedPath()));
    connect(nameListButton,SIGNAL(clicked(bool)),this,SLOT(getNameList()));
    connect(nameListEdit,SIGNAL(textChanged(QString)),this,SLOT(changeNameList(QString)));
    connect(startButton,SIGNAL(clicked(bool)),this,SLOT(startGenerate()));
    connect(exitButton,SIGNAL(clicked(bool)),this,SLOT(close()));
}

void BLMainWindow::getSeedPath()
{
    seedPathStr = QFileDialog::getOpenFileName(this,tr("打开源文件"),"","jpg files(*.jpg);;png files(*.png)");
    if(!seedPathStr.isEmpty())
    {
         if(!seedImage->load(seedPathStr))
         {
             QMessageBox::critical(this,tr("错误"),tr("不能打开文件"));
             seedPath->setText("");
             seedLoadFlag = false;
             return;
         }
         seedLoadFlag = true;
    }
    seedPath->setText(seedPathStr);
}

void BLMainWindow::getNameList()
{
    nameListPathStr = QFileDialog::getOpenFileName(this,tr("打开源文件"),"","txt files(*.txt)");
    if(!nameListPathStr.isEmpty())
    {
        QFile file(nameListPathStr);
        if(!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::critical(this,tr("错误"),tr("不能打开文件"));
            nameList = "";
            nameListEdit->setText("");
            return;
        }
        QTextStream ins(&file);
        while(!ins.atEnd())
        {
            nameList += ins.readLine();
            nameList += "/";
        }
        file.close();
        nameListEdit->setText(nameList);
    }
}

void BLMainWindow::changeNameList(QString name_list)
{
    nameList = name_list;
}

void BLMainWindow::startGenerate()
{
    if(!seedLoadFlag)
    {
        QMessageBox::critical(this,tr("错误"),tr("源文件未加载"));
        return;
    }
    if(nameList.isEmpty())
    {
        QMessageBox::critical(this,tr("错误"),tr("名单为空"));
        return;
    }
    savePath = QFileDialog::getSaveFileName(this,tr("保存生成的文件"),"/","zip file(*.zip)");
    if(!savePath.isEmpty())
    {
        QFile file(savePath);
        if(file.open(QIODevice::ReadOnly))
        {
            QMessageBox::critical(this,tr("错误"),tr("文件已存在"));
            savePath = "";
            return;
        }
    }
    else
    {
        QMessageBox::critical(this,tr("错误"),tr("文件已存在"));
        savePath = "";
        return;
    }
    //xianchengkaishi
    thread->setArgumentsPathStr(seedPathStr,nameList,savePath);
    thread->run();
}


void BLMainWindow::setStyle(const QString &style)
{
    QFile qss(style);
    qss.open(QFile::ReadOnly);
    qApp->setStyleSheet(qss.readAll());
    qss.close();
}




