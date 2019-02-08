#include "mainwindow.hh"

#include <QApplication>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLineEdit>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QVBoxLayout>

#include "char_sets_picker.hh"

// for getting tab to work nicely and and example of keypressevent override
// https://stackoverflow.com/questions/39146509/pressing-tab-in-qtextedit-in-dialog-change-behavior

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

    setMinimumSize(400, 100);
    text = new PracticeArea(this);

    auto vlayout = new QVBoxLayout;
    vlayout->addWidget(text);

    auto hlayout = new QHBoxLayout;
    hlayout->addLayout(vlayout);

    auto central = new QWidget;
    central->setLayout(hlayout);

    setCentralWidget(central);

    picker = new CharacterSetsPicker(this);
    picker->setWindowFlag(Qt::Window);

    connect(picker, &CharacterSetsPicker::checkbox_changed, this,
            &MainWindow::update_characters);

    QMenu *e_menu = menuBar()->addMenu(tr("&Edit"));
    e_menu->addAction("New sentence", text, &PracticeArea::new_sentence);
    e_menu->addAction("Custom characters", this,
                      &MainWindow::custom_characters);
    e_menu->addAction("Character sets", this, &MainWindow::character_sets);

    QMenu *h_menu = menuBar()->addMenu("Help");
    h_menu->addAction("About", this, &MainWindow::about);
    h_menu->addAction("About &Qt", qApp, &QApplication::aboutQt);

    statusBar()->showMessage(QStringLiteral("Errors: 0"));
    connect(text, &PracticeArea::errors_updated, [this]() {
        this->statusBar()->showMessage(
                QStringLiteral("Errors %1").arg(text->get_errors()));
    });
}

void MainWindow::about() {
    QMessageBox::about(this, "About",
                       "A practice tutor which makes you practice the "
                       "combinations that are the hardest for you.");
}

void MainWindow::custom_characters() {
    bool ok;
    QString in = QInputDialog::getText(this, "Custom characters",
                                       "Enter custom characters",
                                       QLineEdit::Normal, QString{}, &ok);
    if (ok)
        text->set_chars(in);
}

void MainWindow::character_sets() { picker->show(); }

void MainWindow::update_characters() {
    // TODO this should be moved to PracticeArea and char set picker should
    // contain letters associated with each checkbox. It could return just
    // empty string when isChecked returns false. Also chars should be added
    // dynamically.
    QString chars = u8"";
    if (picker->checkbox_lower->isChecked())
        chars += u8"qwertyuiopasdfghjklzxcvbnm";
    if (picker->checkbox_upper->isChecked())
        chars += u8"QWERTYUIOPASDFGHJKLZXCVBNM";
    if (picker->checkbox_sym->isChecked())
        chars += u8R"(`~!@#$%^&*()-_=+{[]};:'"\|,<.>/?)";
    if (picker->checkbox_num->isChecked())
        chars += u8"0123456789";

    if (chars.isEmpty())
        chars = QString(u8"qwertyuiopasdfghjklzxcvbnm");

    text->set_chars(chars);
}
