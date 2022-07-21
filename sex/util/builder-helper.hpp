#pragma once

#define AddBuilderFieldDefault(Type, ExternalName, DefaultValue)   \
private:                                        \
    Type ExternalName = DefaultValue;           \
                                                \
public:                                         \
    Type Get##ExternalName() const {            \
        return ExternalName;                    \
    }                                           \
                                                \
    auto& Set##ExternalName(Type __new_val) {   \
        ExternalName = __new_val;               \
        return *this;                           \
    }

#define AddBuilderField(Type, ExternalName) AddBuilderFieldDefault(Type, ExternalName, {})