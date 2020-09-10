#pragma once

#include <QObject>
#include <QQuickItem>
#include <QString>
#include <memory>

class Shader : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString vertex READ vertex WRITE setVertex NOTIFY vertexChanged)
    Q_PROPERTY(QString fragment READ fragment WRITE setFragment NOTIFY fragmentChanged)

public:
    Shader() { }
    Shader(const QString& vertex, const QString& fragment)
    {
        m_vertex = vertex;
        m_fragment = fragment;
    }

    QString vertex() const
    {
        return m_vertex;
    }
    QString fragment() const
    {
        return m_fragment;
    }
public slots:
    void setVertex(QString vertex)
    {
        if (m_vertex == vertex)
            return;

        m_vertex = vertex;
        emit vertexChanged(m_vertex);
    }

    void setFragment(QString fragment)
    {
        if (m_fragment == fragment)
            return;

        m_fragment = fragment;
        emit fragmentChanged(m_fragment);
    }

signals:
    void vertexChanged(QString vertex);

    void fragmentChanged(QString fragment);

private:
    QString m_vertex;
    QString m_fragment;
};

class ShaderLibrary : public QQuickItem {
    Q_OBJECT
    Q_DISABLE_COPY(ShaderLibrary)

    Q_PROPERTY(Shader* lightning READ lightning WRITE setLightning NOTIFY lightningChanged)

public:
    explicit ShaderLibrary(QQuickItem* parent = nullptr);
    ~ShaderLibrary() override;

    Shader* lightning() const
    {
        return m_lightning.get();
    }

public slots:
    void setLightning(Shader* lightning)
    {
        if (m_lightning.get() == lightning)
            return;

        m_lightning.reset(lightning);
        emit lightningChanged(m_lightning.get());
    }

signals:
    void lightningChanged(Shader* lightning);

private:
    std::unique_ptr<Shader> m_lightning;
};
