#include "char_sets_picker.hh"

#include <QGroupBox>
#include <QVBoxLayout>

CharacterSetsPicker::CharacterSetsPicker(QWidget *parent) : QWidget(parent) {
    setWindowTitle("Character sets");
    QGroupBox *chars_box = new QGroupBox(this);

    checkbox_lower = new QCheckBox("Lowercase", chars_box);
    checkbox_upper = new QCheckBox("Uppercase", chars_box);
    checkbox_sym = new QCheckBox("Symbols", chars_box);
    checkbox_num = new QCheckBox("Numbers", chars_box);

    checkbox_lower->setChecked(true);

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->addWidget(checkbox_lower);
    vlayout->addWidget(checkbox_upper);
    vlayout->addWidget(checkbox_sym);
    vlayout->addWidget(checkbox_num);
    chars_box->setLayout(vlayout);

    setFixedSize(vlayout->sizeHint());

    for (auto &&el :
         {checkbox_lower, checkbox_upper, checkbox_num, checkbox_sym})
        connect(el, &QCheckBox::stateChanged, this,
                &CharacterSetsPicker::checkbox_changed);
}
