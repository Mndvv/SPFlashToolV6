#include "HWInfoTree.h"

HWInfoTree::HWInfoTree(const QString &name, const QString &value):
    m_name(name), m_value(value)
{

}

const QString &HWInfoTree::getName() const
{
    return m_name;
}

const QString &HWInfoTree::getValue() const
{
    return m_value;
}

HWInfoLeafNode::HWInfoLeafNode(const QString &name, const QString &value):
    HWInfoTree(name, value)
{

}

bool HWInfoLeafNode::hasChildren() const
{
    return false;
}

int HWInfoLeafNode::count() const
{
    return 0;
}

const HWInfoTree::HWInfoTree_ptr HWInfoLeafNode::getChildren(int index) const
{
    Q_UNUSED(index)
    return HWInfoTree_ptr(nullptr);
}

bool HWInfoContainerNode::hasChildren() const
{
    return m_children.count() > 0;
}

int HWInfoContainerNode::count() const
{
    return m_children.count();
}

const HWInfoTree::HWInfoTree_ptr HWInfoContainerNode::getChildren(int index) const
{
    if (index >= 0 && index < m_children.count()) {
        return m_children.at(index);
    }
    return HWInfoTree_ptr(nullptr);
}

HWInfoContainerNode::HWInfoContainerNode():
    HWInfoTree(QStringLiteral("unknown-name"), QStringLiteral("unknown-value"))
{

}

HWInfoContainerNode::HWInfoContainerNode(const QString &name, const QString &value):
    HWInfoTree(name, value)
{

}

void HWInfoContainerNode::add(const HWInfoTree_ptr &tree_node)
{
    m_children.append(tree_node);
}
