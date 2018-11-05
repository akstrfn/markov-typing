#ifndef CHAR_SETS_PICKER_H
#define CHAR_SETS_PICKER_H

#include <QWidget>
#include <QCheckBox>

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
