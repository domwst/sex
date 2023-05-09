#pragma once

#define AddBuilderFieldDefault(Type, ExternalName, DefaultValue)   \
private:                                        \
    Type ExternalName = DefaultValue;           \
                                                \
public:                                         \
    Type get##ExternalName() const {            \
        return ExternalName;                    \
    }                                           \
                                                \
    auto& set##ExternalName(Type __new_val) {   \
        ExternalName = __new_val;               \
        return *this;                           \
    }

#define AddBuilderField(Type, ExternalName) AddBuilderFieldDefault(Type, ExternalName, {})