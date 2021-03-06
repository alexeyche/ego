#pragma once

#include <ego/base/base.h>
#include <ego/util/string.h>
#include <ego/util/log/log.h>

#include <iostream>

namespace NEgo {

	template <typename T>
    T ParseParenthesis(TString str, std::function<T(TString)> childCb, std::function<T(TString, TVector<T>)> parentCb, TString parent = "") {
    	size_t fp = str.find_first_of("(");
        size_t lp = str.find_last_of(")");
        if((fp == TString::npos)&&(lp == TString::npos)) {
        	return childCb(str);
        }
        if((fp != TString::npos)&&(lp == TString::npos)) {
            throw TErrException() << "Got only open parenthesis, need close one\n";
        }
        if((lp != TString::npos)&&(fp == TString::npos)) {
            throw TErrException() << "Got only close parenthesis, need open one\n";
        }

		TString newParent = NStr::Trim(str.substr(0, fp));
        TString inner = NStr::Trim(str.substr(fp+1, lp-fp-1));

        TVector<TString> childSpecs = NStr::Split(inner, ",", "()");
        ENSURE(childSpecs.size()>0, "Got composite function " << newParent << " without child\n");

        TVector<T> childOuts;
        for(const auto& subInner: childSpecs) {
        	T v = ParseParenthesis(subInner, childCb, parentCb, newParent);
        	childOuts.push_back(v);
        }

        return parentCb(newParent, childOuts);
    }

}