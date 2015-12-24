#pragma once

#include <ego/base/base.h>
#include <ego/base/errors.h>

#include <ego/protos/extension.pb.h>
#include <ego/util/string.h>

#include <vector>
#include <map>
#include <iostream>

namespace NEgo {

    template <typename Proto>
    class TProtoOptions {
    public:
        TProtoOptions(int argc, const char** argv, TString description) : Description(description) {
            for(size_t i=1; i<argc; ++i) {
                Opts.push_back(argv[i]);
            }

            const google::protobuf::Descriptor* descriptor = Proto::descriptor();
            int count = descriptor->field_count();
            for (int i = 0; i < count; ++i) {
                const google::protobuf::FieldDescriptor* fieldDesc = descriptor->field(i);
                TString long_option = CamelCaseToOption(fieldDesc->name());

                NamedFields.insert(std::make_pair(long_option, fieldDesc));

                TString short_option = fieldDesc->options().GetExtension(NEgoProto::short_option);
                if(!short_option.empty()) {
                    ShortNamedFields.insert(std::make_pair(short_option, fieldDesc));
                }
            }
        }

        bool Parse(Proto& message) const {
            if(Opts.size() == 0) {
                Usage();
                return false;
            }

            for(const auto& o: Opts) {
                if((o == "-h")&&("--help")) {
                    Usage();
                    return false;
                }
            }
            for(auto iter=Opts.begin(); iter != Opts.end(); ++iter) {
                {
                    auto ptr = NamedFields.find(*iter);
                    if(ptr != NamedFields.end()) {
                        SetMessageValue(message, ptr->second, iter, Opts.end());
                        continue;
                    }
                }
                {
                    auto ptr = ShortNamedFields.find(*iter);
                    if(ptr != ShortNamedFields.end()) {
                        SetMessageValue(message, ptr->second, iter, Opts.end());
                        continue;
                    }
                }
            }

            return true;
        }

        void SetMessageValue(Proto& message, const google::protobuf::FieldDescriptor* desc,
                std::vector<TString>::const_iterator &value_iterator, std::vector<TString>::const_iterator end_iter) const {
            ENSURE(value_iterator != end_iter, "Got end of options");

            const google::protobuf::Reflection* reflection = message.GetReflection();
            switch(desc->type()) {
                case google::protobuf::FieldDescriptor::TYPE_BOOL:
                    reflection->SetBool(&message, desc, true);
                    break;
                case google::protobuf::FieldDescriptor::TYPE_STRING:
                    {
                        ++value_iterator;
                        if(value_iterator == end_iter) {
                            throw TEgoException() << "Can't find value for option " << desc->name();
                        }
                        reflection->SetString(&message, desc, *value_iterator);
                    }
                    break;
                default:
                    throw TEgoException() << "Unknown protobuf type: " << desc->type();
            }

        }

        void Usage() const {
            std::cout << Description << "\n\n";
            const google::protobuf::Descriptor* descriptor = Proto::descriptor();
            int count = descriptor->field_count();
            for (int i = 0; i < count; ++i) {
                auto* fieldDesc = descriptor->field(i);
                std::cout << "\t" << CamelCaseToOption(fieldDesc->name());

                TString short_option = fieldDesc->options().GetExtension(NEgoProto::short_option);
                if(!short_option.empty()) {
                    std::cout << ", " << short_option;
                }

                TString desc = fieldDesc->options().GetExtension(NEgoProto::description);
                if(!desc.empty()) {
                    std::cout << "\t" << desc;
                }
                std::cout << "\n";
            }
        }

    private:
        std::vector<TString> Opts;

        std::map<TString, const google::protobuf::FieldDescriptor*> NamedFields;
        std::map<TString, const google::protobuf::FieldDescriptor*> ShortNamedFields;

        std::string Description;
    };



} // NEgo