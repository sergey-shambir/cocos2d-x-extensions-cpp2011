
// Do not edit!
// File was generated with activerecord-codegen utility.


#include "SampleGameData.h"
#include "../../utils/ccdictionarybuilder.h"

SoldierRecord::SoldierRecord()
    : health(0)
    , fatigue(0)
{
}

void SoldierRecord::saveDataTo(CCDictionary *dict) const
{
    CCDictionaryBuilder h(dict);
    h.setInt(health, "health");
    h.setInt(fatigue, "fatigue");
}

void SoldierRecord::loadDataFrom(CCDictionary *dict)
{
    DictionaryReader h(dict);
    health = h.getInt("health");
    fatigue = h.getInt("fatigue");
}

int SoldierRecord::getHealth() const
{
    return health;
}

int SoldierRecord::getFatigue() const
{
    return fatigue;
}


void SoldierRecord::setHealth(int var)
{
    health = var;
}

void SoldierRecord::setFatigue(int var)
{
    fatigue = var;
}


NinjaRecord::NinjaRecord()
    : m_hasKatana(false)
{
}

void NinjaRecord::saveDataTo(CCDictionary *dict) const
{
    SoldierRecord::saveDataTo(dict);
    CCDictionaryBuilder h(dict);
    h.setBool(m_hasKatana, "hasKatana");
}

void NinjaRecord::loadDataFrom(CCDictionary *dict)
{
    SoldierRecord::loadDataFrom(dict);
    DictionaryReader h(dict);
    m_hasKatana = h.getBool("hasKatana");
}

bool NinjaRecord::hasKatana() const
{
    return m_hasKatana;
}


void NinjaRecord::setHasKatana(bool var)
{
    m_hasKatana = var;
}


MerchantRecord::MerchantRecord()
    : m_acceptsBarter(false)
    , moneyAmount(0)
    , honor(0)
{
}

void MerchantRecord::saveDataTo(CCDictionary *dict) const
{
    CCDictionaryBuilder h(dict);
    h.setBool(m_acceptsBarter, "acceptsBarter");
    h.setInt(moneyAmount, "moneyAmount");
    h.setInt(honor, "honor");
}

void MerchantRecord::loadDataFrom(CCDictionary *dict)
{
    DictionaryReader h(dict);
    m_acceptsBarter = h.getBool("acceptsBarter");
    moneyAmount = h.getInt("moneyAmount");
    honor = h.getInt("honor");
}

bool MerchantRecord::acceptsBarter() const
{
    return m_acceptsBarter;
}

int MerchantRecord::getMoneyAmount() const
{
    return moneyAmount;
}

int MerchantRecord::getHonor() const
{
    return honor;
}


void MerchantRecord::setAcceptsBarter(bool var)
{
    m_acceptsBarter = var;
}

void MerchantRecord::setMoneyAmount(int var)
{
    moneyAmount = var;
}

void MerchantRecord::setHonor(int var)
{
    honor = var;
}



