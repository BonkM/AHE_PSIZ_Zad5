#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <string>

uint8_t read_byte(std::istream& is) {
  char c = 0;
  if (!is.good()) {
    throw std::runtime_error(is.eof() ? "unexpected EOF" : "unexpected read error");
  }
  is.get(c);
  if (!is.good()) {
    throw std::runtime_error(is.eof() ? "unexpected EOF" : "unexpected read error");
  }
  return c;
}

uint64_t read_uint(std::istream& is, uint8_t bytes) {
  int64_t res = 0;
  switch (bytes) {
    case 8:
      res |= ((uint64_t)read_byte(is)) << (8*(bytes-8));
      // fallthrough
    case 7:
      res |= ((uint64_t)read_byte(is)) << (8*(bytes-7));
      // fallthrough
    case 6:
      res |= ((uint64_t)read_byte(is)) << (8*(bytes-6));
      // fallthrough
    case 5:
      res |= ((uint64_t)read_byte(is)) << (8*(bytes-5));
      // fallthrough
    case 4:
      res |= ((uint64_t)read_byte(is)) << (8*(bytes-4));
      // fallthrough
    case 3:
      res |= ((uint64_t)read_byte(is)) << (8*(bytes-3));
      // fallthrough
    case 2:
      res |= ((uint64_t)read_byte(is)) << (8*(bytes-2));
      // fallthrough
    case 1:
      res |= ((uint64_t)read_byte(is)) << (8*(bytes-1));
      break;
    default:
      throw std::runtime_error("too many bytes to fit into uint32_t");
  }
  // switch (bytes) {
  //   case 8:
  //     res |= ((uint64_t)read_byte(is)) << 56;
  //     // fallthrough
  //   case 7:
  //     res |= ((uint64_t)read_byte(is)) << 48;
  //     // fallthrough
  //   case 6:
  //     res |= ((uint64_t)read_byte(is)) << 40;
  //     // fallthrough
  //   case 5:
  //     res |= ((uint64_t)read_byte(is)) << 32;
  //     // fallthrough
  //   case 4:
  //     res |= ((uint64_t)read_byte(is)) << 24;
  //     // fallthrough
  //   case 3:
  //     res |= ((uint64_t)read_byte(is)) << 16;
  //     // fallthrough
  //   case 2:
  //     res |= ((uint64_t)read_byte(is)) << 8;
  //     // fallthrough
  //   case 1:
  //     res |= ((uint64_t)read_byte(is)) << 0;
  //     break;
  //   default:
  //     throw std::runtime_error("too many bytes to fit into uint32_t");
  // }
  return res;
}

uint64_t read_uint64(std::istream& is) {
  return read_uint(is, 8);
}

int32_t read_int32(std::istream& is) {
  return read_uint(is, 4);
}

uint32_t read_uint32(std::istream& is) {
  return read_uint(is, 4);
}

uint16_t read_uint16(std::istream& is) {
  return read_uint(is, 2);
}

void skip(std::istream& is, std::streamoff n = 1) {
  is.seekg(n, std::ios_base::cur);
  if (is.fail()) {
    throw std::runtime_error("unexpected read error");
  }
}

struct bitmap_file_header {
  uint16_t file_identifier;
  uint32_t file_size;
  uint16_t reserved_a;
  uint16_t reserved_b;
  uint32_t pixels_offset;

  static bitmap_file_header read(std::istream& is) {
    bitmap_file_header h;
    h.file_identifier = read_uint16(is);
    h.file_size = read_uint32(is);
    h.reserved_a = read_uint16(is);
    h.reserved_b = read_uint16(is);
    h.pixels_offset = read_uint32(is);
    return h;
  };

  std::ostream& format(std::ostream& out, std::string_view prefix, std::string_view indent) {
    out << '{' << '\n';
    out << prefix << indent << "\"file_identifier\": " << this->file_identifier << ",\n";
    out << prefix << indent << "\"file_size\": " << this->file_size << ",\n";
    out << prefix << indent << "\"reserved_a\": " << this->reserved_a << ",\n";
    out << prefix << indent << "\"reserved_b\": " << this->reserved_b << ",\n";
    out << prefix << indent << "\"pixels_offset\": " << this->pixels_offset << "\n";
    out << prefix << '}';
    return out;
  };
};

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "Usage: bitmapinfo <bitmap path>" << std::endl;
    return 1;
  }

  std::ifstream file(argv[1]);
  std::cout << "\"bitmap_file_header\": ";
  bitmap_file_header::read(file).format(std::cout, "", "  ") << std::endl;
  return 0;
}
