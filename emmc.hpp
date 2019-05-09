#ifndef EMMC_HPP
#define EMMC_HPP
#include <cstdint>
#include <fstream>

class Interrupt9;

struct SD_DATA32_IRQ
{
    bool data32;
    bool rd32rdy_irq_pending;
    bool tx32rq_irq_pending;
    bool rd32rdy_irq_enable;
    bool tx32rq_irq_enable;
};

enum MMC_State
{
    MMC_Idle = 0,
    MMC_Ready,
    MMC_Identify,
    MMC_Standby,
    MMC_Transfer,
    MMC_Data,
    MMC_Receive,
    MMC_Program
};

class EMMC
{
    private:
        std::ifstream nand, sd;
        std::ifstream* cur_transfer_drive;
        Interrupt9* int9;
        bool app_command;
        uint16_t port_select;
        uint32_t istat, imask;

        SD_DATA32_IRQ sd_data32;

        MMC_State state;

        uint32_t argument;
        uint32_t response[4];

        uint32_t ocr_reg;
        uint32_t regcsd[4];
        uint8_t nand_csd[16];
        uint32_t nand_cid[4], sd_cid[4];

        uint8_t regsd_status[64];
        uint8_t nand_block[1024];

        uint32_t cmd_block_len;

        uint16_t data_block_len;
        uint16_t data_blocks;
        uint16_t data32_block_len;
        uint16_t data32_blocks;

        uint16_t ctrl;

        uint8_t regscr[8];
        uint8_t* transfer_buffer;
        uint32_t transfer_size;
        uint32_t transfer_pos;
        uint32_t transfer_blocks;
        uint32_t transfer_start_addr;
        bool block_transfer;

        void send_cmd(int command);
        void send_acmd(int command);

        uint16_t read_fifo();
        uint32_t read_fifo32();

        bool nand_selected();
        uint32_t get_csr();
        uint32_t get_r1_reply();
        uint32_t get_sdr1_reply();
        void command_end();
        void transfer_end();
        void data_ready();
        void set_istat(uint32_t field);
    public:
        EMMC(Interrupt9* int9);
        ~EMMC();

        bool mount_nand(std::string file_name);
        bool mount_sd(std::string file_name);
        void load_cid(uint8_t* cid);
        void reset();

        uint16_t read16(uint32_t addr);
        uint32_t read32(uint32_t addr);
        void write16(uint32_t addr, uint16_t value);
};

bool inline EMMC::nand_selected()
{
    return port_select & 0x1;
}

#endif // EMMC_HPP
