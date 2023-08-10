// Coordinates between the LoadFromFs task and the events that discover new blocks
class BlockManager
{
    void addBlock(
        BinData data,
        uint64_t reconciliation,
        uint32_t shard,
        uint32_t block);
}
