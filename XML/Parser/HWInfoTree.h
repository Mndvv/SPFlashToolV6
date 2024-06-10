#ifndef HWINFOTREE_H
#define HWINFOTREE_H

#include <QString>
#include <QVector>
#include <QSharedPointer>

class HWInfoTree
{
public:
    using HWInfoTree_ptr = QSharedPointer<HWInfoTree>;

public:
    HWInfoTree(const QString &name, const QString &value);
    virtual ~HWInfoTree() {}

    const QString &getName() const;
    const QString &getValue() const;

    virtual bool hasChildren() const = 0;
    virtual int count() const = 0;
    virtual const HWInfoTree_ptr getChildren(int index) const = 0;

private:
    QString m_name;
    QString m_value;
};

class HWInfoLeafNode: public HWInfoTree
{
public:
    HWInfoLeafNode(const QString &name, const QString &value);

    virtual bool hasChildren() const override;
    virtual int count() const override;
    virtual const HWInfoTree_ptr getChildren(int index) const override;
};

class HWInfoContainerNode: public HWInfoTree
{
public:
    HWInfoContainerNode();
    HWInfoContainerNode(const QString &name, const QString &value);

    void add(const HWInfoTree_ptr &tree_node);

    virtual bool hasChildren() const override;
    virtual int count() const override;
    virtual const HWInfoTree_ptr getChildren(int index) const override;

private:
    QVector<HWInfoTree_ptr> m_children;
};

#endif // HWINFOTREE_H
