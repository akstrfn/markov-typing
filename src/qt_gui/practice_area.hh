#ifndef PRACTICE_AREA_H
#define PRACTICE_AREA_H

#include <vector>

#include <QTextEdit>
#include "../probability_matrix.hh"

class PracticeArea : public QTextEdit {
    Q_OBJECT
private:
    QTextCharFormat format;
    QFont font;
    QTextCursor::MoveOperation direction;
    QTextCursor cursor;
    QString current_chars = u8"qwertyuiopasdfghjklzxcvbnm";
    ProbabilityMatrix matrix{current_chars};
    int cursor_pos{};
    QVector<int> allowed_keys = {Qt::Key_Space, Qt::Key_Return, Qt::Key_Backspace};

    std::vector<int> errors_vec{};
    int errors{};

    void update_errors();

public:
    PracticeArea(QWidget* = nullptr);
    void set_chars(QString);
    void new_sentence();
    int get_errors();

protected:
    void keyPressEvent(QKeyEvent*);

    // Disable mouse
    void mousePressEvent(QMouseEvent*);
    void mouseDoubleClickEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);

signals:
    void errors_updated();
};

#endif /* PRACTICE_AREA_H */
