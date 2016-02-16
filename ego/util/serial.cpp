#include "serial.h"

namespace NEgo {

    TSerializer::TSerializer(google::protobuf::Message& message, ESerialMode mode)
        : Message(message)
        , Mode(mode)
    {
        Refl = Message.GetReflection();
        Descr = Message.GetDescriptor();
    }

    void TSerializer::operator() (ui32& v, int protoField) {
        switch (Mode) {
            case ESerialMode::IN:
            {
                v = Refl->GetUInt32(Message, Descr->FindFieldByNumber(protoField));
            }
            break;
            case ESerialMode::OUT:
            {
                Refl->SetUInt32(&Message, Descr->FindFieldByNumber(protoField), v);
            }
            break;
        }
    }

    void TSerializer::operator() (TVector<double>& v, int protoField) {
        TVectorD laVec = NLa::StdToVec(v);
        (*this)(laVec, protoField);
        v = NLa::VecToStd(laVec);
    }

    void TSerializer::operator() (TVectorD& v, int protoField) {
        switch (Mode) {
            case ESerialMode::IN:
            {
                const NEgoProto::TVectorD* vecProto = GetMessage<NEgoProto::TVectorD>(protoField);
                v = TVectorD(vecProto->x_size());
                for (size_t vecIdx=0; vecIdx < v.size(); ++vecIdx) {
                    v(vecIdx) = vecProto->x(vecIdx);
                }
            }
            break;
            case ESerialMode::OUT:
            {
                NEgoProto::TVectorD* vecProto = GetMutMessage<NEgoProto::TVectorD>(protoField);
                for (const auto& val: v) {
                    vecProto->add_x(val);
                }
            }
            break;
        }
    }

    void TSerializer::operator() (TMatrixD& m, int protoField) {
        switch (Mode) {
            case ESerialMode::IN:
            {
                const NEgoProto::TMatrixD* mat = GetMessage<NEgoProto::TMatrixD>(protoField);
                m = TMatrixD(mat->n_rows(), mat->n_cols());
                for (size_t rowIdx=0; rowIdx < m.n_rows; ++rowIdx) {
                    const NEgoProto::TVectorD& row = mat->row(rowIdx);
                    for (size_t colIdx=0; colIdx < m.n_cols; ++colIdx) {
                        m(rowIdx, colIdx) = row.x(colIdx);
                    }
                }
            }
            break;
            case ESerialMode::OUT:
            {
                NEgoProto::TMatrixD* mat = GetMutMessage<NEgoProto::TMatrixD>(protoField);
                mat->set_n_rows(m.n_rows);
                mat->set_n_cols(m.n_cols);
                for (size_t rowIdx=0; rowIdx < m.n_rows; ++rowIdx) {
                    NEgoProto::TVectorD* row = mat->add_row();
                    for (size_t colIdx=0; colIdx < m.n_cols; ++colIdx) {
                        row->add_x(m(rowIdx, colIdx));
                    }
                }
            }
            break;
        }
    }

    void TSerializer::operator() (google::protobuf::Message& m, int protoField) {
        switch (Mode) {
            case ESerialMode::IN:
            {
                const google::protobuf::Message& messageField = Refl->GetMessage(Message, Descr->FindFieldByNumber(protoField));
                m.CopyFrom(messageField);
            }
            break;
            case ESerialMode::OUT:
            {
                google::protobuf::Message* messageField = Refl->MutableMessage(&Message, Descr->FindFieldByNumber(protoField));
                messageField->CopyFrom(m);
            }
            break;
        }
    }

    bool TSerializer::IsInput() const {
        return Mode == ESerialMode::IN;
    }

    bool TSerializer::IsOutput() const {
        return Mode == ESerialMode::OUT;
    }

} // namespace NEgo