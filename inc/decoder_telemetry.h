#ifndef DECODER_TELEMETRY_H
#define DECODER_TELEMETRY_H
#include <cstdint>
#include <iostream>
#include <print>

struct DecoderTelemetry {
#ifdef ENABLE_TELEMETRY
  uint64_t retired_instructions = 0;
  uint64_t decoded_instructions = 0;
  uint64_t fast_lookups = 0;
  uint64_t slow_lookups = 0;
  uint64_t blocks_created = 0;

  constexpr void on_retire() noexcept { retired_instructions++; }
  constexpr void on_decode() noexcept { decoded_instructions++; }
  constexpr void on_fast_lookups() noexcept { fast_lookups++; }
  constexpr void on_slow_lookups() noexcept { slow_lookups++; }
  constexpr void on_blocks_created() noexcept { blocks_created++; }

  ~DecoderTelemetry() {
    std::println(std::cout, "\n--- Decoder telemetry ---");
    std::println(std::cout, "Retired: {:L}", retired_instructions);
    std::println(std::cout, "Decoded: {:L}", decoded_instructions);
    std::println(std::cout, "Blocks created: {} w/ avg. {} instructions/block",
                 blocks_created,
                 static_cast<double>(decoded_instructions) /
                     static_cast<double>(blocks_created));
    std::println(std::cout, "Chained lookup: {:L}", fast_lookups);
    std::println(std::cout, "Slow lookup: {:L}", slow_lookups);
  }
#else
  constexpr void on_retire() noexcept {}
  constexpr void on_decode() noexcept {}
  constexpr void on_fast_lookups() noexcept {}
  constexpr void on_slow_lookups() noexcept {}
  constexpr void on_blocks_created() noexcept {}
#endif
};
#endif