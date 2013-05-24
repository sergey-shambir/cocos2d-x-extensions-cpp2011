
struct ISerializableRecord
{
    virtual ~ISerializableRecord() {}
    virtual void saveDataTo(CCDictionary *dict) const = 0;
    virtual void loadDataFrom(CCDictionary *dict) = 0;
};

struct SoldierRecord : public ISerializableRecord
{
    SoldierRecord();
    void saveDataTo(CCDictionary *dict) const;
    void loadDataFrom(CCDictionary *dict);
    int getHealth() const;
    int getFatigue() const;

    void setHealth(int);
    void setFatigue(int);

protected:
    int health;
    int fatigue;
};

struct NinjaRecord : public SoldierRecord
{
    NinjaRecord();
    void saveDataTo(CCDictionary *dict) const;
    void loadDataFrom(CCDictionary *dict);
    bool hasKatana() const;

    void setHasKatana(bool);

protected:
    bool m_hasKatana;
};

struct MerchantRecord : public ISerializableRecord
{
    MerchantRecord();
    void saveDataTo(CCDictionary *dict) const;
    void loadDataFrom(CCDictionary *dict);
    bool acceptsBarter() const;
    int getMoneyAmount() const;
    int getHonor() const;

    void setAcceptsBarter(bool);
    void setMoneyAmount(int);
    void setHonor(int);

protected:
    bool m_acceptsBarter;
    int moneyAmount;
    int honor;
};

