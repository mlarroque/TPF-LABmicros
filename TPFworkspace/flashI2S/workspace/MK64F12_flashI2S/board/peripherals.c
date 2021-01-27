/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Peripherals v9.0
processor: MK64FN1M0xxx12
package_id: MK64FN1M0VLL12
mcu_data: ksdk2_0
processor_version: 9.0.0
board: FRDM-K64F
functionalGroups:
- name: BOARD_InitPeripherals
  UUID: 7098e75b-69cf-47c6-9df9-5bb077fddf69
  called_from_default_init: true
  selectedCore: core0
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/

/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
component:
- type: 'system'
- type_id: 'system'
- global_system_definitions:
  - user_definitions: ''
  - user_includes: ''
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/***********************************************************************************************************************
 * Included files
 **********************************************************************************************************************/
#include "peripherals.h"

/***********************************************************************************************************************
 * BOARD_InitPeripherals functional group
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DMA initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'DMA'
- type: 'edma'
- mode: 'basic'
- custom_name_enabled: 'false'
- type_id: 'edma_a23fca76a894e1bcdf9d01a687505ff9'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'DMA'
- config_sets:
  - fsl_edma:
    - common_settings:
      - enableContinuousLinkMode: 'true'
      - enableHaltOnError: 'true'
      - enableRoundRobinArbitration: 'true'
      - enableDebugMode: 'true'
    - dma_table:
      - 0: []
    - edma_channels:
      - 0:
        - edma_channel:
          - channel_prefix_id: 'audio_transport'
          - eDMAn: '0'
          - eDMA_source: 'kDmaRequestMux0I2S0Tx'
          - enableTriggerPIT: 'false'
          - enable_custom_name: 'false'
        - init_callback: 'true'
        - callback_function: 'callback_dma_config'
        - callback_user_data: 'p2userdata'
        - init_preemption: 'false'
        - preemption:
          - enableChannelPreemption: 'false'
          - enablePreemptAbility: 'false'
          - channelPriority: '0'
        - init_channel_link: 'true'
        - channel_link: 'kEDMA_MajorLink'
        - linkedChannel: '0'
        - init_bandwidth: 'false'
        - bandwidth: 'kEDMA_BandwidthStallNone'
        - init_interrupts: 'true'
        - channel_enabled_interrupts: 'kEDMA_MajorInterruptEnable'
        - interrupt_channel:
          - IRQn: 'DMA0_IRQn'
          - enable_interrrupt: 'enabled'
          - enable_priority: 'false'
          - priority: '0'
          - enable_custom_name: 'false'
        - tcd_pool_enable: 'true'
        - tcd_settings:
          - tcd_size: '2'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
const edma_config_t DMA_config = {
  .enableContinuousLinkMode = true,
  .enableHaltOnError = true,
  .enableRoundRobinArbitration = true,
  .enableDebugMode = true
};

  /* Channel audio_transport global variables */
edma_handle_t DMA_audio_transport_Handle;
edma_tcd_t * DMA_audio_transport_TCD_pool_ptr;

static void DMA_init(void) {

  /* Channel audio_transport initialization */
  /* Set the source kDmaRequestMux0I2S0Tx request in the DMAMUX */
  DMAMUX_SetSource(DMA_DMAMUX_BASEADDR, DMA_AUDIO_TRANSPORT_DMA_CHANNEL, DMA_AUDIO_TRANSPORT_DMA_REQUEST);
  /* Enable the channel 0 in the DMAMUX */
  DMAMUX_EnableChannel(DMA_DMAMUX_BASEADDR, DMA_AUDIO_TRANSPORT_DMA_CHANNEL);
  /* Create the eDMA DMA_audio_transport_Handle handle */
  EDMA_CreateHandle(&DMA_audio_transport_Handle, DMA_DMA_BASEADDR, DMA_AUDIO_TRANSPORT_DMA_CHANNEL);
  EDMA_SetCallback(&DMA_audio_transport_Handle, callback_dma_config, p2userdata);
  EDMA_SetChannelLink(DMA_DMA_BASEADDR, DMA_AUDIO_TRANSPORT_DMA_CHANNEL, kEDMA_MajorLink, 0);
  EDMA_EnableChannelInterrupts(DMA_DMA_BASEADDR, DMA_AUDIO_TRANSPORT_DMA_CHANNEL, kEDMA_MajorInterruptEnable);
  /* Enable interrupt DMA0_IRQn request in the NVIC. */
  EnableIRQ(DMA_DMA_CH_INT_DONE_0_IRQN);
  /* Allocate TCD memory pool */
  DMA_audio_transport_TCD_pool_ptr = (edma_tcd_t *)malloc(sizeof(edma_tcd_t) * (DMA_AUDIO_TRANSPORT_TCD_SIZE + 1));
  if (DMA_audio_transport_TCD_pool_ptr != NULL) {
    EDMA_InstallTCDMemory(&DMA_audio_transport_Handle, (edma_tcd_t *)((uint32_t)(DMA_audio_transport_TCD_pool_ptr + 1) & (~0x1FU)), DMA_AUDIO_TRANSPORT_TCD_SIZE);
  }
}

/***********************************************************************************************************************
 * I2S0 initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'I2S0'
- type: 'sai'
- mode: 'interrupts'
- custom_name_enabled: 'false'
- type_id: 'sai_e171ee1d4e17db4b5b234f946b59a148'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'I2S0'
- config_sets:
  - fsl_sai:
    - mclk_config:
      - masterClockSource: 'kSAI_MclkSourceSysclk'
      - masterClockSourceFreq: 'BOARD_BootClockRUN'
      - masterClockFrequency: '6.144 MHz'
    - usage: 'playback'
    - whole:
      - tx_group:
        - sai_config:
          - protocol: 'kSAI_BusI2S'
          - syncMode: 'kSAI_ModeAsync'
          - bitClockSource: 'kSAI_BclkSourceMclkDiv'
        - transfer_format:
          - sampleRate_Hz: 'kSAI_SampleRate8KHz'
          - bitWidth: 'kSAI_WordWidth8bits'
          - stereo: 'kSAI_Stereo'
          - isFrameSyncCompact: 'false'
          - watermark: '2'
          - channelMask: 'kSAI_Channel0Mask'
        - interrupt_sel: 'kSAI_SyncErrorInterruptEnable kSAI_FIFORequestInterruptEnable'
        - interrupt:
          - IRQn: 'I2S0_Tx_IRQn'
          - enable_interrrupt: 'enabled'
          - enable_priority: 'false'
          - priority: '0'
          - enable_custom_name: 'false'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
/* I2S0 Tx configuration */
const sai_config_t I2S0_tx_config = {
  .protocol = kSAI_BusI2S,
  .syncMode = kSAI_ModeAsync,
  .mclkOutputEnable = true,
  /* MCLK clock source */
  .mclkSource = kSAI_MclkSourceSysclk,
  .bclkSource = kSAI_BclkSourceMclkDiv, //kSAI_BclkSourceMclkOption1
  .masterSlave = kSAI_Master
};
/* I2S0 Tx  transfer format */
sai_transfer_format_t I2S0_tx_format = {
  .sampleRate_Hz = kSAI_SampleRate8KHz,
  .bitWidth = kSAI_WordWidth8bits,
  .stereo = kSAI_Stereo,
  .masterClockHz = 6144000UL,
  .watermark = 2U,
  .channel = 0U,
  .protocol = kSAI_BusI2S,
  .isFrameSyncCompact = false
};

static void I2S0_init(void) {
  /* Initialize SAI Tx sub-module functionality */
  SAI_TxInit(I2S0_PERIPHERAL, &I2S0_tx_config);
  /* Initialize SAI Tx transfer format */
  SAI_TxSetFormat(I2S0_PERIPHERAL, &I2S0_tx_format, I2S0_TX_MCLK_SOURCE_CLOCK_HZ, I2S0_TX_BCLK_SOURCE_CLOCK_HZ);
  /* Enable selected Tx interrupts */
  SAI_TxEnableInterrupts(I2S0_PERIPHERAL, (kSAI_SyncErrorInterruptEnable | kSAI_FIFORequestInterruptEnable));
  /* Enable interrupt I2S0_Tx_IRQn request in the NVIC. */
  EnableIRQ(I2S0_SERIAL_TX_IRQN);
}

/***********************************************************************************************************************
 * Initialization functions
 **********************************************************************************************************************/
void BOARD_InitPeripherals(void)
{
  /* Global initialization */
  DMAMUX_Init(DMA_DMAMUX_BASEADDR);
  EDMA_Init(DMA_DMA_BASEADDR, &DMA_config);

  /* Initialize components */
  DMA_init();
  I2S0_init();
}

/***********************************************************************************************************************
 * BOARD_InitBootPeripherals function
 **********************************************************************************************************************/
void BOARD_InitBootPeripherals(void)
{
  BOARD_InitPeripherals();
}
