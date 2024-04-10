#include "AttributeSet.hh"
#include "Deserializer.hh"
#include "Utilities/Assert.hh"

#include <algorithm>

// TODO: Debug
#include <iostream>

namespace nir {

/// If `s` is defined, returns it as an instance of `optional<U>`.
template<typename T, typename U>
std::optional<T> narrowed_optional(std::optional<U> const& s) {
  return s.has_value() ? s.value().template as<T>() : std::nullopt;
}

/// Returns `s` as an instance of `vector<U>`.
template<typename T, typename U>
std::vector<T> narrowed_sequence(std::vector<U> const& s) {
  std::vector<T> r;
  r.reserve(s.size());
  std::transform(s.begin(), s.end(), r.begin(), [](auto const& e) {
    return e.template as<T>().value();
  });
  return r;
}

template<typename T, typename F>
requires std::invocable<F, Deserializer&>
std::vector<T> Deserializer::sequence(F&& decode) {
  auto s = source.unsigned_leb128().get();
  std::vector<T> result;
  result.reserve(s);

  for (std::size_t i = 0; i < s; ++i) {
    result.push_back(decode(*this));
  }

  return result;
}

template<typename T, typename F>
requires std::invocable<F, Deserializer&>
T Deserializer::internable(std::vector<T>& memo, F&& decode) {
  if (source.peek() == -1) {
    source.u8();
    return memo[source.unsigned_leb128().get()];
  } else {
    auto p = source.current_position();
    auto v = decode(*this);
    if (source.current_position() > (p + 2)) {
      memo.push_back(v);
    }
    return v;
  }
}

template<typename T, typename F>
requires std::invocable<F, Deserializer&>
std::optional<T> Deserializer::optional(F&& decode) {
  if (boolean()) {
    return decode(*this);
  } else {
    return std::nullopt;
  }
}

Symbol Deserializer::symbol() {
  return internable<Symbol>(interned_symbols, [](auto& self) {
    switch (self.source.u8().get()) {
      case raw_value(tag::Symbol::none):
        return Symbol::none();

      case raw_value(tag::Symbol::top):
        return Symbol(symbol::Top(self.string()));

      case raw_value(tag::Symbol::member):
        return Symbol(symbol::Member{
          self.symbol().template as<symbol::Top>().value(),
          self.signature()
        });

      default:
        fatal_error("unexpected tag");
    }
  });
}

Signature Deserializer::signature() {
  return Signature{string()};
}

Definition Deserializer::definition() {
  auto tag = source.u8().get();
  auto attributes = AttributeSet::contents_of(
    sequence<Attribute>([](auto& self) { return self.attribute(); }));

  switch (tag) {
    case raw_value(tag::Definition::variable):
    case raw_value(tag::Definition::constant):
      return Definition(definition::Binding{
        attributes,
        symbol().as<symbol::Member>().value(),
        type(),
        value(),
        tag == raw_value(tag::Definition::constant),
        source_position()
      });

    case raw_value(tag::Definition::declare):
      return Definition(definition::Forward{
        attributes,
        symbol().as<symbol::Member>().value(),
        type().as<type::Function>().value(),
        source_position()
      });

    case raw_value(tag::Definition::define):
      return Definition(definition::Method{
        attributes,
        symbol().as<symbol::Member>().value(),
        type().as<type::Function>().value(),
        sequence<Instruction>([](auto& self) { return self.instruction(); }),
        debug(),
        source_position()
      });

    case raw_value(tag::Definition::trait):
      return Definition(definition::Trait{
        attributes,
        symbol().as<symbol::Top>().value(),
        narrowed_sequence<symbol::Top>(sequence<Symbol>([](auto& self) { return self.symbol(); })),
        source_position()
      });

    case raw_value(tag::Definition::class_):
      return Definition(definition::Class{
        attributes,
        symbol().as<symbol::Top>().value(),
        narrowed_optional<symbol::Top>(optional<Symbol>([](auto& self) { return self.symbol(); })),
        narrowed_sequence<symbol::Top>(sequence<Symbol>([](auto& self) { return self.symbol(); })),
        source_position()
      });

    case raw_value(tag::Definition::module):
      return Definition(definition::Module{
        attributes,
        symbol().as<symbol::Top>().value(),
        narrowed_optional<symbol::Top>(optional<Symbol>([](auto& self) { return self.symbol(); })),
        narrowed_sequence<symbol::Top>(sequence<Symbol>([](auto& self) { return self.symbol(); })),
        source_position()
      });

    default:
      fatal_error("unexpected tag");
  }
}

definition::Method::DebugInformation Deserializer::debug() {
  return definition::Method::DebugInformation{
    local_name(),
    sequence<LexicalScope>([](auto& self) { return self.lexical_scope(); })
  };
}

LexicalScope Deserializer::lexical_scope() {
  return LexicalScope{
    scope_identifier(),
    scope_identifier(),
    source_position()
  };
}

std::unordered_map<Local, std::string> Deserializer::local_name() {
  auto count = source.unsigned_leb128().get();
  std::unordered_map<Local, std::string> r;
  r.reserve(count);
  while (count > 0) {
    auto k = local();
    auto v = string();
    r[k] = v;
    count -= 1;
  }
  return r;
}

Instruction Deserializer::instruction() {
  switch (source.u8().get()) {
    case raw_value(tag::Instruction::label):
      return Instruction(instruction::Label{
        local(),
        sequence<value::Local>([](auto& self) { return self.label_argument(); }),
        source_position()
      });

    case raw_value(tag::Instruction::let):
      return Instruction(instruction::Let{
        local(), operation(), next(), source_position(), scope_identifier()
      });

    case raw_value(tag::Instruction::unwind):
      fatal_error("unexpected tag");

    case raw_value(tag::Instruction::return_):
      return Instruction(instruction::Return{
        value(), source_position()
      });

    case raw_value(tag::Instruction::jump):
      return Instruction(instruction::Jump{
        next(), source_position()
      });

    case raw_value(tag::Instruction::if_):
      return Instruction(instruction::If{
        value(), next(), next(), source_position()
      });

    case raw_value(tag::Instruction::switch_):
      return Instruction(instruction::Switch{
        value(),
        sequence<Next>([](auto& self) { return self.next(); }),
        source_position()
      });

    case raw_value(tag::Instruction::throw_):
      return Instruction(instruction::Throw{
        value(), next(), source_position()
      });

    case raw_value(tag::Instruction::unreachable):
      return Instruction(instruction::Unreachable{
        next(), source_position()
      });

    case raw_value(tag::Instruction::linktime_if):
      return Instruction(instruction::LinktimeJump{
        linktime_condition(), next(), next(), source_position()
      });

    default:
      fatal_error("unexpected tag");
  }
}

Type Deserializer::type() {
  return internable<Type>(interned_types, [](auto& self) {
    switch (self.source.u8().get()) {
      case raw_value(tag::Type::vararg):
        return Type::vararg();
      case raw_value(tag::Type::boolean):
        return Type::u1();
        case raw_value(tag::Type::pointer):
        return Type::pointer();
      case raw_value(tag::Type::char_):
        return Type::u16();
      case raw_value(tag::Type::byte):
        return Type::i8();
      case raw_value(tag::Type::short_):
        return Type::i16();
      case raw_value(tag::Type::int_):
        return Type::i32();
      case raw_value(tag::Type::long_):
        return Type::i64();
      case raw_value(tag::Type::float_):
        return Type::f32();
      case raw_value(tag::Type::double_):
        return Type::f64();
      case raw_value(tag::Type::array_value):
        return Type(type::ArrayValue{
          self.type(),
          self.source.unsigned_leb128().get()
        });

      case raw_value(tag::Type::struct_value):
        return Type(type::Struct{
          self.template sequence<Type>([](auto& self) { return self.type(); })
        });

      case raw_value(tag::Type::function):
        return Type(type::Function{
          self.template sequence<Type>([](auto& self) { return self.type(); }),
          self.type()
        });

      case raw_value(tag::Type::null):
        return Type::null();
      case raw_value(tag::Type::nothing):
        return Type::nothing();
      case raw_value(tag::Type::virtual_):
        return Type::virtual_();
      case raw_value(tag::Type::var):
        return Type(type::Var(self.type()));
      case raw_value(tag::Type::unit):
        return Type::unit();
      case raw_value(tag::Type::array):
        return Type(type::ArrayReference{
          self.type(),
          self.boolean()
        });

      case raw_value(tag::Type::reference):
        return Type(type::Reference{
          self.symbol().template as<symbol::Top>().value(),
          self.boolean(),
          self.boolean()
        });

      case raw_value(tag::Type::size):
        return Type::size();

      default:
        fatal_error("unexpected tag");
    }
  });
}

Value Deserializer::value() {
  return internable<Value>(interned_values, [](auto& self) {
    switch (self.source.u8().get()) {
      case raw_value(tag::Value::true_):
        return Value(value::Boolean(true));
      case raw_value(tag::Value::false_):
        return Value(value::Boolean(false));
      case raw_value(tag::Value::null):
        return Value(value::Null());
      case raw_value(tag::Value::zero):
        return Value(value::Zero(self.type()));
      case raw_value(tag::Value::char_):
        return Value((value::Char)(self.source.unsigned_leb128().get() & 0xffff));
      case raw_value(tag::Value::byte):
        return Value((value::Byte)(self.source.i8().get()));
      case raw_value(tag::Value::short_):
        return Value((value::Short)(self.source.signed_leb128().get() & 0xffff));
      case raw_value(tag::Value::int_):
        return Value((value::Int)(self.source.signed_leb128().get() & 0xffffffff));
      case raw_value(tag::Value::long_):
        return Value((value::Long)(self.source.signed_leb128().get()));
      case raw_value(tag::Value::float_):
        return Value(value::Float(self.source.f32().get()));
      case raw_value(tag::Value::double_):
        return Value(value::Double(self.source.f64().get()));
      case raw_value(tag::Value::struct_):
        return Value(value::Struct({
          self.template sequence<Value>([](auto& self) { return self.value(); })
        }));
      case raw_value(tag::Value::array):
        return Value(value::ArrayValue({
          self.type(),
          self.template sequence<Value>([](auto& self) { return self.value(); })
        }));
      case raw_value(tag::Value::byte_string):
        return Value(value::ByteString(self.bytes()));
      case raw_value(tag::Value::local):
        return Value(value::Local{
          self.local(),
          self.type()
        });
      case raw_value(tag::Value::symbol):
        return Value(value::Symbol({
          self.symbol(),
          self.type()
        }));
      case raw_value(tag::Value::unit):
        return Value(value::Unit());
      case raw_value(tag::Value::constant):
        return Value(value::Constant(self.value()));
      case raw_value(tag::Value::string):
        return Value(value::String(self.string()));
      case raw_value(tag::Value::virtual_):
        return Value(value::Virtual(self.source.unsigned_leb128().get()));
      case raw_value(tag::Value::class_of):
        return Value(value::ClassOf(self.symbol().template as<symbol::Top>().value()));
      case raw_value(tag::Value::linktime_condition):
        fatal_error("unexpected tag");
      case raw_value(tag::Value::size):
        return Value(value::Size(self.source.unsigned_leb128().get()));
      default:
        fatal_error("unexpected tag");
    }
  });
}

value::Local Deserializer::label_argument() {
  return value().as<value::Local>().value();
}

Next Deserializer::next() {
  switch (source.u8().get()) {
    case raw_value(tag::Next::none):
      return Next::none();
    case raw_value(tag::Next::unwind):
      return Next(next::Unwind{label_argument(), next()});
    case raw_value(tag::Next::case_):
      return Next(next::Case{value(), next()});
    case raw_value(tag::Next::label):
      return Next(next::Label{
        local(),
        sequence<Value>([](auto& self) { return self.value(); })
      });
    default:
      fatal_error("unexpected tag");
  }
}

Operation Deserializer::operation() {
  switch (source.u8().get()) {
    case raw_value(tag::Operation::call):
      return Operation(operation::Call{
        type().as<type::Function>().value(),
        value(),
        sequence<Value>([](auto& self) { return self.value(); })
      });

    case raw_value(tag::Operation::load):
      return Operation(operation::Load{
        type(), value(), memory_order()
      });

    case raw_value(tag::Operation::store):
      return Operation(operation::Store{
        type(), value(), value(), memory_order()
      });

    case raw_value(tag::Operation::element):
      return Operation(operation::Element{
        type(),
        value(),
        sequence<uint32_t>([](auto& self) { return self.uint32(); })
      });

    case raw_value(tag::Operation::extract):
      return Operation(operation::Extract{
        value(),
        sequence<uint32_t>([](auto& self) { return self.uint32(); })
      });

    case raw_value(tag::Operation::insert):
      return Operation(operation::Insert{
        value(),
        value(),
        sequence<uint32_t>([](auto& self) { return self.uint32(); })
      });

    case raw_value(tag::Operation::stackalloc):
      return Operation(operation::StackAllocate{
        type(),
        source.unsigned_leb128().get()
      });

    case raw_value(tag::Operation::binary):
      return Operation(operation::BinaryApply{
        binary_operator(), type(), value(), value()
      });

    case raw_value(tag::Operation::compare):
      return Operation(operation::Compare{
        comparison_operator(), type(), value(), value()
      });

    case raw_value(tag::Operation::convert):
      return Operation(operation::Convert{
        conversion_operator(), type(), value()
      });

    case raw_value(tag::Operation::fence):
      return Operation(operation::Fence{
        memory_order()
      });

    case raw_value(tag::Operation::classalloc):
      return Operation(operation::ClassAllocate{
        symbol().as<symbol::Top>().value(),
        optional<Value>([](auto& self) { return self.value(); })
      });

    case raw_value(tag::Operation::fieldload):
      return Operation(operation::FieldLoad{
        type(),
        value(),
        symbol().as<symbol::Member>().value()
      });

    case raw_value(tag::Operation::fieldstore):
      return Operation(operation::FieldStore{
        type(),
        value(),
        symbol().as<symbol::Member>().value(),
        value()
      });

    case raw_value(tag::Operation::field):
      return Operation(operation::Field{
        value(),
        symbol().as<symbol::Member>().value()
      });

    case raw_value(tag::Operation::method):
      return Operation(operation::Method{
        value(), signature()
      });

    case raw_value(tag::Operation::dynmethod):
      return Operation(operation::DynamicMethod{
        value(), signature()
      });

    case raw_value(tag::Operation::module):
      return Operation(operation::Module{
        symbol().as<symbol::Top>().value()
      });

    case raw_value(tag::Operation::as):
      return Operation(operation::As{
        type(), value()
      });

    case raw_value(tag::Operation::is):
      return Operation(operation::Is{
        type(), value()
      });

    case raw_value(tag::Operation::copy):
      return Operation(operation::Copy{
        value()
      });

    case raw_value(tag::Operation::size_of):
      return Operation(operation::SizeOf{
        type()
      });

    case raw_value(tag::Operation::alignment_of):
      return Operation(operation::AlignmentOf{
        type()
      });

    case raw_value(tag::Operation::box):
      return Operation(operation::Box{
        type(), value()
      });

    case raw_value(tag::Operation::unbox):
      return Operation(operation::Unbox{
        type(), value()
      });

    case raw_value(tag::Operation::var):
      return Operation(operation::Var{
        type()
      });

    case raw_value(tag::Operation::varload):
      return Operation(operation::VarLoad{
        value(),
      });

    case raw_value(tag::Operation::varstore):
      return Operation(operation::VarStore{
        value(), value()
      });

    case raw_value(tag::Operation::arrayalloc):
      return Operation(operation::ArrayAllocate{
        type(),
        value(),
        optional<Value>([](auto& self){ return self.value(); })
      });

    case raw_value(tag::Operation::arrayload):
      return Operation(operation::ArrayLoad{
        type(), value(), uint32()
      });

    case raw_value(tag::Operation::arraystore):
      return Operation(operation::ArrayStore{
        type(), value(), uint32(), value()
      });

    case raw_value(tag::Operation::arraylength):
      return Operation(operation::ArrayLength{
        value()
      });

    default:
      fatal_error("unexpected tag");
  }
}

LinktimeCondition Deserializer::linktime_condition() {
  fatal_error("not implemented");
}

BinaryOperator Deserializer::binary_operator() {
  switch (source.u8().get()) {
    case raw_value(tag::BinaryOperator::iadd):
      return BinaryOperator::iadd;
    case raw_value(tag::BinaryOperator::fadd):
      return BinaryOperator::fadd;
    case raw_value(tag::BinaryOperator::isub):
      return BinaryOperator::isub;
    case raw_value(tag::BinaryOperator::fsub):
      return BinaryOperator::fsub;
    case raw_value(tag::BinaryOperator::imul):
      return BinaryOperator::imul;
    case raw_value(tag::BinaryOperator::fmul):
      return BinaryOperator::fmul;
    case raw_value(tag::BinaryOperator::sdiv):
      return BinaryOperator::sdiv;
    case raw_value(tag::BinaryOperator::udiv):
      return BinaryOperator::udiv;
    case raw_value(tag::BinaryOperator::fdiv):
      return BinaryOperator::fdiv;
    case raw_value(tag::BinaryOperator::srem):
      return BinaryOperator::srem;
    case raw_value(tag::BinaryOperator::urem):
      return BinaryOperator::urem;
    case raw_value(tag::BinaryOperator::frem):
      return BinaryOperator::frem;
    case raw_value(tag::BinaryOperator::shl):
      return BinaryOperator::shl;
    case raw_value(tag::BinaryOperator::lshr):
      return BinaryOperator::lshr;
    case raw_value(tag::BinaryOperator::ashr):
      return BinaryOperator::ashr;
    case raw_value(tag::BinaryOperator::and_):
      return BinaryOperator::and_;
    case raw_value(tag::BinaryOperator::or_):
      return BinaryOperator::or_;
    case raw_value(tag::BinaryOperator::xor_):
      return BinaryOperator::xor_;
    default:
      fatal_error("unexpected tag");
  }
}

ComparisonOperator Deserializer::comparison_operator() {
  switch (source.u8().get()) {
    case raw_value(tag::ComparisonOperator::ieq):
      return ComparisonOperator::ieq;
    case raw_value(tag::ComparisonOperator::ine):
      return ComparisonOperator::ine;
    case raw_value(tag::ComparisonOperator::ugt):
      return ComparisonOperator::ugt;
    case raw_value(tag::ComparisonOperator::uge):
      return ComparisonOperator::uge;
    case raw_value(tag::ComparisonOperator::ult):
      return ComparisonOperator::ult;
    case raw_value(tag::ComparisonOperator::ule):
      return ComparisonOperator::ule;
    case raw_value(tag::ComparisonOperator::sgt):
      return ComparisonOperator::sgt;
    case raw_value(tag::ComparisonOperator::sge):
      return ComparisonOperator::sge;
    case raw_value(tag::ComparisonOperator::slt):
      return ComparisonOperator::slt;
    case raw_value(tag::ComparisonOperator::sle):
      return ComparisonOperator::sle;
    case raw_value(tag::ComparisonOperator::feq):
      return ComparisonOperator::feq;
    case raw_value(tag::ComparisonOperator::fne):
      return ComparisonOperator::fne;
    case raw_value(tag::ComparisonOperator::fgt):
      return ComparisonOperator::fgt;
    case raw_value(tag::ComparisonOperator::fge):
      return ComparisonOperator::fge;
    case raw_value(tag::ComparisonOperator::flt):
      return ComparisonOperator::flt;
    case raw_value(tag::ComparisonOperator::fle):
      return ComparisonOperator::fle;
    default:
      fatal_error("unexpected tag");
  }
}

ConversionOperator Deserializer::conversion_operator() {
  switch (source.u8().get()) {
    case raw_value(tag::ConversionOperator::trunc):
      return ConversionOperator::trunc;
    case raw_value(tag::ConversionOperator::zext):
      return ConversionOperator::zext;
    case raw_value(tag::ConversionOperator::sext):
      return ConversionOperator::sext;
    case raw_value(tag::ConversionOperator::fptrunc):
      return ConversionOperator::fptrunc;
    case raw_value(tag::ConversionOperator::fpext):
      return ConversionOperator::fpext;
    case raw_value(tag::ConversionOperator::fptoui):
      return ConversionOperator::fptoui;
    case raw_value(tag::ConversionOperator::fptosi):
      return ConversionOperator::fptosi;
    case raw_value(tag::ConversionOperator::uitofp):
      return ConversionOperator::uitofp;
    case raw_value(tag::ConversionOperator::sitofp):
      return ConversionOperator::sitofp;
    case raw_value(tag::ConversionOperator::ptrtoint):
      return ConversionOperator::ptrtoint;
    case raw_value(tag::ConversionOperator::inttoptr):
      return ConversionOperator::inttoptr;
    case raw_value(tag::ConversionOperator::bitcast):
      return ConversionOperator::bitcast;
    case raw_value(tag::ConversionOperator::ssize_cast):
      return ConversionOperator::ssize_cast;
    case raw_value(tag::ConversionOperator::zsize_cast):
      return ConversionOperator::zsize_cast;
    default:
      fatal_error("unexpected tag");
  }
}

MemoryOrder Deserializer::memory_order() {
  auto v = source.u8().get();
  precondition((v >= 0) && (v <= 5), "unexpected tag");
  return static_cast<MemoryOrder>(v);
}

Local Deserializer::local() {
  return Local(source.unsigned_leb128().get());
}

Attribute Deserializer::attribute() {
  using namespace attribute;

  switch (source.u8().get()) {
    case raw_value(tag::Attribute::may_inline):
      return Attribute(Kind::may_inline);
    case raw_value(tag::Attribute::inline_hint):
      return Attribute(Kind::inline_hint);
    case raw_value(tag::Attribute::no_inline):
      return Attribute(Kind::no_inline);
    case raw_value(tag::Attribute::always_inline):
      return Attribute(Kind::always_inline);

    case raw_value(tag::Attribute::may_specialize):
      return Attribute(Kind::may_specialize);
    case raw_value(tag::Attribute::no_specialize):
      return Attribute(Kind::no_specialize);

    case raw_value(tag::Attribute::un_opt):
      return Attribute(Kind::un_opt);
    case raw_value(tag::Attribute::no_opt):
      return Attribute(Kind::no_opt);
    case raw_value(tag::Attribute::did_opt):
      return Attribute(Kind::did_opt);
    case raw_value(tag::Attribute::bail_opt):
      return Attribute(BailOpt{string()});

    case raw_value(tag::Attribute::dyn):
      return Attribute(Kind::dyn);
    case raw_value(tag::Attribute::stub):
      return Attribute(Kind::stub);
    case raw_value(tag::Attribute::extern_):
      return Attribute(Extern{boolean()});
    case raw_value(tag::Attribute::link):
      return Attribute(Link{string()});
    case raw_value(tag::Attribute::define):
      return Attribute(Define{string()});
    case raw_value(tag::Attribute::abstract):
      return Attribute(Kind::abstract);
    case raw_value(tag::Attribute::volatile_):
      return Attribute(Kind::volatile_);
    case raw_value(tag::Attribute::final):
      return Attribute(Kind::final);
    case raw_value(tag::Attribute::safe_publish):
      return Attribute(Kind::safe_publish);

    case raw_value(tag::Attribute::link_time_resolved):
      return Attribute(Kind::link_time_resolved);
    case raw_value(tag::Attribute::uses_intrinsic):
      return Attribute(Kind::uses_intrinsic);

    case raw_value(tag::Attribute::align): {
      auto s = source.signed_leb128().get();
      auto g = optional<std::string>([](auto& self) { return self.string(); });
      return Attribute(Alignment{s, g});
    }

    default:
      fatal_error("unexpected tag");
  }
}

SourcePosition Deserializer::source_position() {
  auto p = string();
  return SourcePosition{
    (p == "") ? SourceFile() : SourceFile(p), // source
    source.unsigned_leb128().get(), // line index
    source.unsigned_leb128().get() // column index
  };
}

ScopeIdentifier Deserializer::scope_identifier() {
  return ScopeIdentifier{source.unsigned_leb128().get()};
}

std::string Deserializer::inline_string() {
  auto n = source.unsigned_leb128().get();
  char points[n];
  auto r = source.bytes(n, static_cast<int8_t*>(static_cast<void*>(points)));
  precondition(n == r, "invalid string");
  return std::string(points, n);
}

std::string Deserializer::string() {
  switch (source.u8().get()) {
    case raw_value(tag::String::empty):
      return std::string();

    case raw_value(tag::String::contained): {
      auto n = source.unsigned_leb128().get();
      auto i = source.unsigned_leb128().get();
      return std::string(interned_strings[i], 0, n);
    }

    case raw_value(tag::String::inserted): {
      auto s = inline_string();
      interned_strings.push_back(s);
      return s;
    }

    case raw_value(tag::String::appended): {
      auto n = source.unsigned_leb128().get();
      auto i = source.unsigned_leb128().get();
      auto s = std::string(interned_strings[i], 0, n) + inline_string();
      interned_strings.push_back(s);
      return s;
    }

    default:
      fatal_error("unexpected tag");
  }
}

std::vector<value::Byte> Deserializer::bytes() {
  auto count = source.unsigned_leb128().get();
  std::vector<value::Byte> result;
  result.reserve(count);
  while (count > 0) {
    result.push_back(source.i8().get());
    count -= 1;
  }
  return result;
}

bool Deserializer::boolean() {
  return source.u8().get() != 0;
}

uint32_t Deserializer::uint32() {
  return (uint32_t)(source.unsigned_leb128().get() & 0xffffffff);
}

} // nir
