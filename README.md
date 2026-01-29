# mod-anti-afk-logout

An AzerothCore module that prevents automatic AFK logout while allowing manual logout.

## The Problem

In WoW 3.3.5a, the **client** initiates a logout request after 30 minutes of inactivity. This is client-side behavior, so there's no simple server config to disable it. This becomes problematic when running AI bots or automation that doesn't generate player input.

## The Solution

This module hooks into the packet handling system and intercepts `CMSG_LOGOUT_REQUEST` packets. When a logout request comes in, it checks if the player has the AFK flag set:

- **AFK flag set** → Automatic logout (blocked)
- **AFK flag not set** → Manual logout (allowed)

When blocking an AFK logout, the module:
1. Sends `SMSG_LOGOUT_CANCEL_ACK` to the client to cancel the logout
2. Optionally clears the AFK flag to prevent spam
3. Keeps the player connected

## Installation

1. Clone this repository into your `azerothcore-wotlk/modules/` directory:
   ```bash
   cd /path/to/azerothcore-wotlk/modules
   git clone https://github.com/yourusername/mod-anti-afk-logout.git
   ```

2. Re-run CMake and rebuild the server:
   ```bash
   cd /path/to/azerothcore-wotlk/build
   cmake ..
   make -j$(nproc)
   ```

3. Copy the configuration file:
   ```bash
   cp modules/mod-anti-afk-logout/conf/mod_anti_afk_logout.conf.dist etc/mod_anti_afk_logout.conf
   ```

4. (Optional) Edit `etc/mod_anti_afk_logout.conf` to customize settings.

5. Restart worldserver.

## Configuration

| Setting | Default | Description |
|---------|---------|-------------|
| `AntiAFKLogout.Enable` | 1 | Enable/disable the module |
| `AntiAFKLogout.OnlyWhenNotInCombat` | 0 | Only block AFK logout when out of combat |
| `AntiAFKLogout.ClearAFKFlag` | 1 | Clear AFK flag when blocking logout |
| `AntiAFKLogout.LogBlocked` | 0 | Log blocked logout attempts |

## How It Works

```
Client idle for 30 min
        ↓
Client sends CMSG_LOGOUT_REQUEST
        ↓
mod-anti-afk-logout intercepts packet
        ↓
    Is player AFK?
      /        \
    Yes         No
     ↓           ↓
  Block      Allow normal
  logout       logout
     ↓
Send SMSG_LOGOUT_CANCEL_ACK
     ↓
Clear AFK flag (optional)
     ↓
Player stays connected
```

## Edge Cases

- **Player types `/logout` while AFK**: Will be blocked. For a private server with bots, this is usually acceptable.
- **Player in combat**: By default, AFK logout is still allowed when in combat (configurable).
- **Manual logout**: Always works because the player won't have the AFK flag set when actively using the client.

## Compatibility

- AzerothCore 3.3.5a (WotLK)
- Should work with mod-playerbots and similar bot modules

## License

This module is released under the GPL V2 license, consistent with AzerothCore.
