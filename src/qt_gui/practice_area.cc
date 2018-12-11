#include "practice_area.hh"

#include <QTextBlock>

#include "../io.hh"
#include "../utils.hh"

PracticeArea::PracticeArea(QWidget *parent) : QTextEdit(parent) {
    font = QFont{"IBM Plex Mono", 12};

    QFontMetrics fm(font);
    setCursorWidth(fm.averageCharWidth());

    font.setStyleHint(QFont::Monospace);
    setFont(font);

    QPalette p = QPalette();
    p.setColor(QPalette::Base, QColor("black"));
    p.setColor(QPalette::Text, QColor("white"));
    setPalette(p);

    new_sentence();
}

void PracticeArea::keyPressEvent(QKeyEvent *e) {

    // disable any char different from the available ones
    // for shift, control est. But this differs between platforms..
    if (e->text() == "")
        return;
    if (!current_chars.contains(e->text()) && !allowed_keys.contains(e->key()))
        return;

    cursor = QTextCursor(this->textCursor());

    // Handle the case when cursor is at the end
    cursor_pos = cursor.position();
    if (cursor_pos == cursor.block().text().size()) {
        if (e->key() == Qt::Key_Return)
            new_sentence();
        return;
    }

    // now disable enter since it has no use anymore
    if (e->key() == Qt::Key_Return)
        return;

    if (cursor_pos == 0)
        timer.start();

    if (e->key() == Qt::Key_Backspace) {
        direction = QTextCursor::Left;
        cursor.movePosition(QTextCursor::Left);
        format.setBackground(QBrush(QColor("black")));
        if (!errors_vec.empty())
            errors_vec.pop_back();
    } else if (e->text() == cursor.block().text().at(cursor_pos)) {
        format.setBackground(QBrush(QColor("green")));
        direction = QTextCursor::Right;
        errors_vec.push_back(0);
        // TODO how is space handled?
        if (cursor_pos != 0) {
            matrix.update_element(
                    cursor.block().text().at(cursor_pos - 1).unicode(),
                    e->text().front().unicode(), timer.elapsed(), true);
        }
    } else {
        format.setBackground(QBrush(QColor("red")));
        direction = QTextCursor::Right;
        errors_vec.push_back(1);
        if (cursor_pos != 0) {
            matrix.update_element(
                    cursor.block().text().at(cursor_pos - 1).unicode(),
                    e->text().front().unicode(), timer.elapsed(), false);
        }
    }

    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
    cursor.setCharFormat(format);

    this->moveCursor(direction);

    update_errors();
    timer.start();
}

void PracticeArea::set_chars(QString chars) {
    // TODO reading and writing like this all the time is very slow... probably
    // the whole json should be kept in memory or at least known character sets
    // for fast check.
    sort_uniq(chars);
    current_chars = chars;
    auto opt_matrix = read_json("data.json", chars.toStdString());
    matrix = opt_matrix.value_or(ProbabilityMatrix(chars));
    new_sentence();
}

void PracticeArea::new_sentence() {
    setTextBackgroundColor(QColor("black"));
    setText(QString(matrix.generate_sentence(40)));
    setAlignment(Qt::AlignCenter);
    errors_vec.clear();
    update_errors();

    write_json("data.json", matrix);
}

void PracticeArea::update_errors() {
    const int tmp = std::accumulate(errors_vec.begin(), errors_vec.end(), 0);
    if (errors != tmp) {
        errors = tmp;
        errors_updated();
    }
}

int PracticeArea::get_errors() { return errors; }

void PracticeArea::mousePressEvent(QMouseEvent *) {}
void PracticeArea::mouseDoubleClickEvent(QMouseEvent *) {}
void PracticeArea::mouseMoveEvent(QMouseEvent *) {}
void PracticeArea::mouseReleaseEvent(QMouseEvent *) {}
