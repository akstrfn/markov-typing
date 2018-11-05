#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "practice_area.hh"
#include "char_sets_picker.hh"

class MainWindow : public QMainWindow {
private:
    PracticeArea *text = nullptr;
    CharacterSetsPicker *picker = nullptr;

public:
    MainWindow(QWidget* = nullptr);
    void about();
    void custom_characters();
    void character_sets();
    void update_characters();
};
#endif /* MAINWINDOW_H */
