#include <linux/module.h>
#include<linux/iio/iio.h>
#include<linux/spi/spi.h>
#include <linux/regulator/consumer.h>
#include <linux/err.h>
#include <linux/delay.h>

 struct mcp3008_chip_info {
	const struct iio_chan_spec *channels;
	unsigned int num_channels;
	unsigned int resolution;
};


struct mcp3008 {
		struct spi_device *spi;
	        struct spi_message msg;
		struct spi_transfer transfer[2];
		struct regulator *reg;
		struct mutex lock;
		const struct mcp3008_chip_info *chip_info;
                u8 tx_buf ____cacheline_aligned;
		u8 rx_buf[2];
};

#define MCP3008_VOLTAGE_CHANNEL(num)				\
		{							\
					.type = IIO_VOLTAGE,				\
					.indexed = 1,					\
					.channel = (num),				\
					.address = (num),				\
					.info_mask_separate = BIT(IIO_CHAN_INFO_RAW),	\
					.info_mask_shared_by_type = BIT(IIO_CHAN_INFO_SCALE) \
				}

#define MCP3008_VOLTAGE_CHANNEL_DIFF(chan1, chan2)		\
		{							\
					.type = IIO_VOLTAGE,				\
					.indexed = 1,					\
					.channel = (chan1),				\
					.channel2 = (chan2),				\
					.address = (chan1),				\
					.differential = 1,				\
					.info_mask_separate = BIT(IIO_CHAN_INFO_RAW),	\
					.info_mask_shared_by_type = BIT(IIO_CHAN_INFO_SCALE) \
				}
static const struct   iio_chan_spec  mcp3008_channels[] = {
         
      MCP3008_VOLTAGE_CHANNEL(0),      
      MCP3008_VOLTAGE_CHANNEL(1),
      MCP3008_VOLTAGE_CHANNEL(2),
      MCP3008_VOLTAGE_CHANNEL(3),
      MCP3008_VOLTAGE_CHANNEL_DIFF(0,1),
      MCP3008_VOLTAGE_CHANNEL_DIFF(1,2),
      MCP3008_VOLTAGE_CHANNEL_DIFF(2,3),
};

static const struct mcp3008_chip_info mcp3008_chip = {
   	
    mcp3008_channels,
      ARRAY_SIZE(mcp3008_channels),
     10 

};

static int mcp3008_adc_conversion(struct mcp3008 *adc, u8 channel,bool differential)
{
		int ret;
		int start_bit= 1;
                adc->rx_buf[0] = 0;
		adc->rx_buf[1] = 0;
                adc->tx_buf = ((start_bit << 6) | (!differential << 5) |
                                                         (channel << 2));
		ret = spi_sync(adc->spi, &adc->msg);
		if (ret < 0)
		return ret;

               return (adc->rx_buf[0] << 2 | adc->rx_buf[1] >> 6);
}

static int mcp3008_read_raw(struct iio_dev *indio_dev,struct iio_chan_spec const *channel, int *val,  int *val2, long mask)
{
		struct mcp3008 *adc = iio_priv(indio_dev);
		int ret = -EINVAL;
	//	int device_index = 0;
                mutex_lock(&adc->lock);
          //      device_index = spi_get_device_id(adc->spi)->driver_data;
                 switch (mask) {
		case IIO_CHAN_INFO_RAW:
												ret = mcp3008_adc_conversion(adc, channel->address,channel->differential);
												if (ret < 0)
												goto out;
                 *val = ret;
											      ret = IIO_VAL_INT;																	  break;
																					case IIO_CHAN_INFO_SCALE:
												ret = regulator_get_voltage(adc->reg);
																						if (ret < 0)
												goto out;
											  /* convert regulator output voltage to mV */
																					 *val = ret / 1000;
											  *val2 = adc->chip_info->resolution;
																					  ret = IIO_VAL_FRACTIONAL_LOG2;
																																break;
												}

          out:
               mutex_unlock(&adc->lock);
               return ret;
}


static const struct iio_info mcp3008_info = {
		.read_raw = mcp3008_read_raw,
		.driver_module = THIS_MODULE,
};


static int mcp3008_probe(struct spi_device *spi)
{
    struct iio_dev *indio_dev;
    struct mcp3008 *adc;
    const struct mcp3008_chip_info *chip_info;
    int ret;
    indio_dev = devm_iio_device_alloc(&spi->dev, sizeof(*adc));
    if (!indio_dev)
          return -ENOMEM;
    adc = iio_priv(indio_dev);
    adc->spi = spi;
    indio_dev->dev.parent = &spi->dev;						   indio_dev->name = spi_get_device_id(spi)->name;
    indio_dev->modes = INDIO_DIRECT_MODE;
    indio_dev->info = &mcp3008_info;
    chip_info = &mcp3008_chip;
    indio_dev->channels = chip_info->channels;
    indio_dev->num_channels = chip_info->num_channels;
    adc->chip_info = chip_info;
    adc->transfer[0].tx_buf = &adc->tx_buf;
    adc->transfer[0].len = sizeof(adc->tx_buf);
    adc->transfer[1].rx_buf = adc->rx_buf;					   adc->transfer[1].len = sizeof(adc->rx_buf);
   spi_message_init_with_transfers(&adc->msg, adc->transfer,ARRAY_SIZE(adc->transfer));
										adc->reg = devm_regulator_get(&spi->dev, "vref");
											if (IS_ERR(adc->reg))
												return PTR_ERR(adc->reg);

     ret = regulator_enable(adc->reg);
if (ret < 0)
											return ret;

  mutex_init(&adc->lock);

   ret = iio_device_register(indio_dev);
											if (ret < 0)
											goto reg_disable;

											return 0;

reg_disable:																					regulator_disable(adc->reg);
											return ret;
}


static int mcp3008_remove(struct spi_device *spi)
{
	struct iio_dev *indio_dev = spi_get_drvdata(spi);
	struct mcp3008 *adc = iio_priv(indio_dev);
        iio_device_unregister(indio_dev);
	regulator_disable(adc->reg);
        return 0;
}

//#if defined(CONFIG_OF)
static const struct of_device_id mcp3008_dt_ids[] = {
                       { 
	 		.compatible = "mcp008",
			.data = &mcp3008_chip,
			},
                       {
			 .compatible = "microchip,mcp008",
			.data = &mcp3008_chip,
			 }, 
		       {
			},

};

//MODULE_DEVICE_TABLE(of, mcp3008_dt_ids);



static struct spi_driver mcp3008_driver = {
   .driver    =	{
		   .name = "mcp008" ,
		   .of_match_table = of_match_ptr(mcp3008_dt_ids),
           	},
         .probe = mcp3008_probe,
	 .remove = mcp3008_remove,

};

static int __init simple_init(void)
{
	pr_info("Namaste %s ..\n", module_name(THIS_MODULE));
        
        __spi_register_driver(THIS_MODULE,&mcp3008_driver);
	return 0;
}

static void __exit simple_exit(void)
{
	spi_unregister_driver(&mcp3008_driver); 
	pr_info("Shubham %s ...\n", module_name(THIS_MODULE));
}

module_init(simple_init);
module_exit(simple_exit);
MODULE_AUTHOR("VENKATESH BABU <gvr0001.vj@gmail.com>");
MODULE_DESCRIPTION("PI_MCP3008_CHANEEL3_SOILMOISTURE");
MODULE_LICENSE("GPL");

