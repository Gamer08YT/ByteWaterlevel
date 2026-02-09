# Automation

The Device Firmware comes with integrated Automations.

- Pump and Fill
- Fill

### Pump and Fill

In this Mode the Device will enable <code>Relay 2</code> if the Level is between the given <code>max</code> and <code>
min</code> Level.

If the Level exceeds the <code>fill</code> Level, <code>Relay 1</code> will be enabled to fill the Tank.

After <code>max</code> Level is reached, <code>Relay 1</code> will be disabled.

### Fill

The mode is the same as the <code>Pump and Fill</code>, but there will be no action for <code>Relay 2</code>, so the
Device will only keep a Fluid Level stable.