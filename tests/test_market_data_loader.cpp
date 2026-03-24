#include <gtest/gtest.h>

#include <vector>

#include "market_data_loader.hpp"
#include "symbol_table.hpp"
#include "tick.hpp"

TEST(market_data_loader, load_sample_ticks_csv) {
    SymbolTable symbol_table;
    std::vector<Tick> ticks = LoadTicksCsv("data/sample_ticks.csv", symbol_table);

    ASSERT_EQ(ticks.size(), 8u);

    EXPECT_EQ(ticks[0].timestamp_us, 1700000000000000LL);
    EXPECT_EQ(ticks[0].symbol_id, 0u);
    EXPECT_DOUBLE_EQ(ticks[0].price, 100.00);
    EXPECT_EQ(ticks[0].volume, 10);

    EXPECT_EQ(ticks[5].timestamp_us, 1700000000000500LL);
    EXPECT_EQ(ticks[5].symbol_id, 0u);
    EXPECT_DOUBLE_EQ(ticks[5].price, 400.15);
    EXPECT_EQ(ticks[5].volume, 7);

    EXPECT_EQ(ticks[6].timestamp_us, 1700000000000600LL);
    EXPECT_EQ(ticks[6].symbol_id, 1u);
    EXPECT_DOUBLE_EQ(ticks[6].price, 10.00);
    EXPECT_EQ(ticks[6].volume, 1);

    EXPECT_EQ(ticks[7].timestamp_us, 1700000000000700LL);
    EXPECT_EQ(ticks[7].symbol_id, 1u);
    EXPECT_DOUBLE_EQ(ticks[7].price, 11.00);
    EXPECT_EQ(ticks[7].volume, 1);

    EXPECT_EQ(symbol_table.get_symbol(0), "TEST");
    EXPECT_EQ(symbol_table.get_symbol(1), "ABC");
}