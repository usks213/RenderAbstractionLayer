/*****************************************************************//**
 * \file   Singleton.h
 * \brief  テンプレートシングルトン
 * 
 * \author USAMI KOSHI
 * \date   2021/06/14
 *********************************************************************/
#pragma once


///@class Singleton
///@brief テンプレートシングルトン
template <class T>
class Singleton {
public:

    ///@brief インスタンス取得
    ///@return インスタンス
    static T& get() {
        static T instance;
        return instance;
    }

protected:
    
    /// @brief デフォルトコンストラクタ
    Singleton() = default;

    /// @brief デフォルトデストラクタ
    virtual ~Singleton() = default;

private:
    Singleton(const Singleton&) = delete;
    Singleton(const Singleton&&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton& operator=(const Singleton&&) = delete;
};
