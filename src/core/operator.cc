#include "core/operator.h"

namespace infini {

bool OperatorNode::isLinearOp() const {
    return enum_to_underlying(type) >= 100 && enum_to_underlying(type) < 200;
}

bool OperatorNode::isElementWiseOp() const {
    return enum_to_underlying(type) >= 200 && enum_to_underlying(type) < 300;
}

bool OperatorNode::isSplitOp() const { return type == OpType::Split; }

bool OperatorNode::isConcatOp() const { return type == OpType::Concat; }

bool OperatorNode::isComputeOp() const {
    return type == OpType::Conv || type == OpType::Matmul ||
           type == OpType::ConvTrans || type == OpType::G2BMM ||
           type == OpType::GBMML;
}

bool OperatorNode::isTransposeOp() const { return type == OpType::Transpose; }

bool OperatorNode::isReshapeOp() const { return type == OpType::Reshape; }

bool OperatorNode::isMemBoundOp() const {
    return type == OpType::MemBound || type == OpType::Activation ||
           type == OpType::Transpose;
}

vector<Shape> MatmulNode::computeShape() const {
    Shape ret{args.b, args.m, args.n};
    return {ret};
}

MatmulNode::MatmulNode(Tensor A, Tensor B, Tensor C, bool transA, bool transB,
                       Tensor bias, ActType act)
    : OperatorNode(OpType::Matmul, {A, B, bias}, {C}),
      args(A->getDims()[0], transA ? A->getDims()[2] : A->getDims()[1],
           transB ? B->getDims()[1] : B->getDims()[2],
           transA ? A->getDims()[1] : A->getDims()[2], transA, transB, act) {
    IT_ASSERT(checkValid(inputs));
}

string MatmulNode::toString() const {
    std::ostringstream os;
    MatmulArgs args = getArgs();
    os << "Matmul([" << (args.transA ? "A^T" : "A") << ","
       << (args.transB ? "B^T" : "B") << ",act=" << (int)args.act
       << "],A=" << inputs[0]->getGuid() << ",B=" << inputs[1]->getGuid()
       << ",C=" << outputs[0]->getGuid() << ")";
    return os.str();
}

bool MatmulNode::checkValid(const TensorVec &inputs) const {
    auto A = inputs[0], B = inputs[1];
    // if (A->getType() == Tensor::Weight && B->getType() == Tensor::Weight)
    //     return false;
    IT_ASSERT(A->getDims().size() == 3 && B->getDims().size() == 3);
    IT_ASSERT(A->getDims()[0] == B->getDims()[0]);
    IT_ASSERT((args.transA ? A->getDims()[1] : A->getDims()[2]) ==
              (args.transB ? B->getDims()[2] : B->getDims()[1]));
    // if (A->getDims().size() != 3 || B->getDims().size() != 3) {
    //     return false;
    // }
    // if (A->getDims()[0] != B->getDims()[0]) {
    //     return false;
    // }
    // if ((args.transA ? A->getDims()[1] : A->getDims()[2]) !=
    //     (args.transB ? B->getDims()[2] : B->getDims()[1])) {
    //     return false;
    // }
    return true;
}

} // namespace infini