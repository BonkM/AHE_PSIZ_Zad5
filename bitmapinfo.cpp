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

struct dib_bitmap_info_header {
  public:
    uint32_t struct_size;
    int32_t pixel_width;
    int32_t pixel_height;
    uint16_t planes;
    uint16_t bits_per_pixel;
    uint32_t compression;
    uint32_t size_in_bytes;
    int32_t horizontal_resolution_pixels_per_meter;
    int32_t vertical_resolution_pixels_per_meter;
    uint32_t number_of_color_indexes;
    uint32_t number_of_required_color_indexes;

    static dib_bitmap_info_header read(std::istream& is) {
      uint32_t struct_size = read_uint32(is);
      switch (struct_size) {
        case 124: // BITMAPV5HEADER
          // fallthrough
        case 108: // BITMAPV4HEADER
          // fallthrough
        case 56: // BITMAPV3INFOHEADER
          // fallthrough
        case 52: // BITMAPV2INFOHEADER
          // fallthrough
        case 40: // BITMAPINFOHEADER
          // fallthrough
          return read_info_header(is, struct_size);

        case 64: // BITMAPCOREHEADER2
          // fallthrough
        case 12: // BITMAPCOREHEADER
          return read_core_header(is, struct_size);

        default:
          throw std::runtime_error("invalid dib struct size");
      }
    };

    std::ostream& format(std::ostream& out, std::string_view prefix, std::string_view indent) {
      out << '{' << '\n';
      out << prefix << indent << "\"struct_size\": " << this->struct_size << ",\n";
      out << prefix << indent << "\"pixel_width\": " << this->pixel_width << ",\n";
      out << prefix << indent << "\"pixel_height\": " << this->pixel_height << ",\n";
      out << prefix << indent << "\"planes\": " << this->planes << ",\n";
      out << prefix << indent << "\"bits_per_pixel\": " << this->bits_per_pixel << ",\n";
      out << prefix << indent << "\"compression\": " << this->compression << ",\n";
      out << prefix << indent << "\"size_in_bytes\": " << this->size_in_bytes << ",\n";
      out << prefix << indent << "\"horizontal_resolution_pixels_per_meter\": " << this->horizontal_resolution_pixels_per_meter << ",\n";
      out << prefix << indent << "\"vertical_resolution_pixels_per_meter\": " << this->vertical_resolution_pixels_per_meter << ",\n";
      out << prefix << indent << "\"number_of_color_indexes\": " << this->number_of_color_indexes << ",\n";
      out << prefix << indent << "\"number_of_required_color_indexes\": " << this->number_of_required_color_indexes << "\n";
      out << prefix << '}';
      return out;
    };

  private:
    static dib_bitmap_info_header read_core_header(std::istream& is, uint32_t size) {
      dib_bitmap_info_header h;
      h.struct_size = size;
      h.pixel_width = read_uint16(is);
      h.pixel_height = read_uint16(is);
      h.planes = read_uint16(is);
      h.bits_per_pixel = read_uint16(is);
      h.compression = 0;
      h.size_in_bytes = 0;
      h.horizontal_resolution_pixels_per_meter = 0;
      h.vertical_resolution_pixels_per_meter = 0;
      h.number_of_color_indexes = 0;
      h.number_of_required_color_indexes = 0;
      return h;
    };

    static dib_bitmap_info_header read_info_header(std::istream& is, uint32_t size) {
      dib_bitmap_info_header h;
      h.struct_size = size;
      h.pixel_width = read_int32(is);
      h.pixel_height = read_int32(is);
      h.planes = read_uint16(is);
      h.bits_per_pixel = read_uint16(is);
      h.compression = read_uint32(is);
      h.size_in_bytes = read_uint32(is);
      h.horizontal_resolution_pixels_per_meter = read_int32(is);
      h.vertical_resolution_pixels_per_meter = read_int32(is);
      h.number_of_color_indexes = read_uint32(is);
      h.number_of_required_color_indexes = read_uint32(is);

      if (h.planes != 1) {
        throw std::runtime_error("invalid number of planes in dib structure");
      }
      if (h.bits_per_pixel != 1 && h.bits_per_pixel != 4 && h.bits_per_pixel != 8 && h.bits_per_pixel != 16 && h.bits_per_pixel != 24 && h.bits_per_pixel != 32) {
        throw std::runtime_error("invalid bits per pixel");
      }
      if (h.compression != 0) {
        throw std::runtime_error("compression is unsupported");
      }
      return h;
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
  std::cout << "\"dib_bitmap_info_header\": ";
  dib_bitmap_info_header::read(file).format(std::cout, "", "  ") << std::endl;
  return 0;
}
