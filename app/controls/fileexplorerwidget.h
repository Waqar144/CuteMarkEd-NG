#ifndef FILEEXPLORERWIDGET_H
#define FILEEXPLORERWIDGET_H

#include <QWidget>

namespace Ui {
class FileExplorerWidget;
}
class QFileSystemModel;
class QSortFilterProxyModel;


class FileExplorerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileExplorerWidget(QWidget *parent = 0);
    ~FileExplorerWidget();

signals:
    void fileSelected(const QString &filePath);

protected:
    void showEvent(QShowEvent *event);

public slots:
    void setPath(const QString &path);

private slots:
    void fileOpen(const QModelIndex& index);
    void onDoubleClick(const QModelIndex& index);
    void oncustomContextMenuRequested(const QPoint p);

private:
    bool initialized;
    Ui::FileExplorerWidget *ui;
    QFileSystemModel *model;
    QSortFilterProxyModel *sortModel;
};

#endif // FILEEXPLORERWIDGET_H
