# Bitrae Core v0.2.0 Release Notes

## Introduction

Bitrae Core version **0.2.0** is now available for download:

🔗 [Release v0.2.0 on GitHub](https://github.com/bitraenet/bitrae/releases/tag/v0.2.0/)

This **minor version update** introduces **new features, security fixes, and important bug fixes** to improve network stability, security, and functionality. Users are encouraged to **upgrade as soon as possible** to benefit from the latest improvements.

Please report any issues via the GitHub issue tracker:

🔗 [Bitrae Issue Tracker](https://github.com/bitraenet/bitrae/issues)

---

## Notable Changes

### 🛡️ Security Updates

This release addresses **critical security vulnerabilities** affecting block propagation, node stability, and external dependencies:

- **[CVE-2024-35202](https://www.cvedetails.com/cve/CVE-2024-35202/): Fix assertions in `PartiallyDownloadedBlock`**
  - Resolves an issue where remote attackers could trigger a **denial of service (DoS)** by sending `blocktxn` messages containing transactions **not committed** in a block's Merkle root.
  - Fix ensures `FillBlock` cannot be called twice for the same `PartiallyDownloadedBlock` instance.
  - **Backported from Bitcoin Core:** [`a8897f6`](https://github.com/bitcoin/bitcoin/commit/a8897f6) → Bitrae commit: `9492448`

- **[Hindered Block Propagation Due to Mutated Blocks](https://bitcoincore.org/en/2024/10/08/disclose-mutated-blocks-hindering-propagation/)**
  - Fixes a flaw where a peer could send **mutated blocks** that incorrectly **cleared the download state of other peers**, leading to **slower block propagation**.
  - **Backported from Bitcoin Core:** [`dbfc748`](https://github.com/bitcoin/bitcoin/commit/dbfc748) → Bitrae commit: `de3ef11`

- **[Infinite Loop Bug in `miniupnp`](https://bitcoincore.org/en/2024/07/31/disclose-upnp-oom/)**
  - Fixes an issue in the `miniupnp` dependency where an attacker on the **local network** could trigger an **out-of-memory (OOM) condition**, potentially crashing affected nodes.
  - **Backported from Bitcoin Core:** [`fa2a5b8`](https://github.com/bitcoin/bitcoin/commit/fa2a5b8) → Bitrae commit: `189e166`

---

### 🔄 Chain Data Updates

- **Updated `checkpointData` and `chainTxData`**
  - **Commit:** `5df0d5c`
  - Added new **checkpoint entries up to block 353328** to improve network security and synchronization speed.
  - Updated `chainTxData` with **recent transaction statistics** to enhance node syncing accuracy and performance.

- **Updated consensus parameters to latest chainwork**
  - **Commit:** `b059883`
  - Set **default assume valid block** to **block 350000** for improved node sync efficiency.
  - Ensures nodes do not verify unnecessary historical transactions, improving performance.

---

### 🐛 Bug Fixes

- **Disable block filters by default when pruning is enabled**  
  - **Commit:** `f9830ba`
  - Prevents potential conflicts by ensuring `-peerblockfilters` and `-blockfilterindex` are disabled when pruning mode is activated.

---

### 🧪 Test-Related Fixes

- **Improved Functional Test Reliability**  
  - **Commit:** `580dd9d`
  - Fixes issues in functional tests to **enhance reliability and accuracy**, ensuring smoother test execution.

---

## 🔥 Contributors

A huge thank you to **all contributors** who helped make this release possible. Your efforts in development, testing, and security research have improved the stability and security of Bitrae Core.

Special thanks to:

- [The Bitcoin Core Developers](https://github.com/bitcoin/bitcoin/)
- [David Burkett](https://github.com/DavidBurkett/)
- [Hector Chu](https://github.com/hectorchu)
- [Loshan](https://github.com/losh11)
- [The Bitrae Core Developers](https://github.com/bitraenet/bitrae/)

---

### 🚀 Upgrade Now

We strongly encourage **all users, miners, and node operators** to upgrade to **Bitrae Core v0.2.0** as soon as possible to take advantage of these fixes and improvements.

📥 **Download Bitrae Core v0.2.0:**  
🔗 [GitHub Releases](https://github.com/bitraenet/bitrae/releases/tag/v0.2.0/)

---

## 💡 Notes for Users
- **If running a pruned node**: Ensure you are aware that block filters (`-peerblockfilters` and `-blockfilterindex`) are now **disabled by default**.
- **For developers**: This release includes backported security fixes from **Bitcoin Core**, ensuring improved stability and security while maintaining compatibility with upstream improvements.

---

## ✅ Summary of Key Fixes
✔ Security vulnerabilities patched  
✔ Improved block propagation  
✔ Updated checkpoint and chain transaction data  
✔ Functional test reliability improvements  
✔ Stability enhancements for pruned nodes  

---

### 🔗 Report Issues
If you experience any issues, please open a ticket on our GitHub issue tracker:  
🔗 [Bitrae Issues](https://github.com/bitraenet/bitrae/issues)

Thank you for supporting **Bitrae Core**! 🚀
