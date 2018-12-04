#ifndef CHAR_SETS_PICKER_H
#define CHAR_SETS_PICKER_H

#include <QWidget>
#include <QCheckBox>

// TODO when opened this class should load all saved sessions in order to be
// able to change them fast
class CharacterSetsPicker : public QWidget {
    Q_OBJECT
public:
    CharacterSetsPicker(QWidget* = nullptr);

    QCheckBox *checkbox_lower;
    QCheckBox *checkbox_upper;
    QCheckBox *checkbox_sym;
    QCheckBox *checkbox_num;

signals:
    void checkbox_changed();
};

#endif /* CHAR_SETS_PICKER_H */
