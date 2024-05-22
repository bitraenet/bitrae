# Bitrae integration/staging tree
================================

https://bitrae.net

## Copyright Notice

This project incorporates work from various developers and projects, with copyrights held as follows:

- © 2009-2014 Bitcoin Developers. All rights reserved.
- © 2011-2024 Litecoin Developers. All rights reserved.
- © 2024 Bitrae Developers. All rights reserved.

The use of this software is subject to the terms and conditions as defined by the respective licenses of these projects.

# Bitrae: A New Era in Cryptocurrency Mining

Welcome to the official documentation for Bitrae, the cryptocurrency designed by miners, for the mining community. Bitrae stands out as a pioneering lightweight version of Bitcoin, crafted to address the needs and preferences of today’s miners. With no pre-mine, Bitrae ensures a level playing field, underscoring our commitment to fairness and decentralization.

## Key Features of Bitrae

- **Scrypt Proof-of-Work Algorithm**: Bitrae adopts the scrypt algorithm for its proof-of-work mechanism, a deliberate choice aimed at ensuring greater mining accessibility. This algorithm is designed to be more memory-intensive, mitigating the dominance of ASIC mining and fostering a more decentralized mining environment.

- **Fast Transaction Confirmations**: With a block target time of 37.5 seconds, Bitrae significantly reduces transaction confirmation times. This enhancement not only improves usability for transactions but also contributes to the network’s capacity to handle more transactions efficiently.

- **Economic Model and Coin Supply**:
  - **Subsidy Halving**: True to the principles of creating a sustainable and deflationary economic model, Bitrae’s mining subsidy halves approximately every 3.36 million blocks, or roughly every four years.
  - **Total Coin Supply**: The total supply of Bitrae coins is capped at approximately 336 million, a decision that strikes a balance between scarcity and accessibility, influencing the long-term value of Bitrae.

- **Mining Rewards and Difficulty Adjustment**:
  - **Reward per Block**: Miners are initially rewarded with 50 Bitrae coins per block, a testament to our commitment to rewarding the mining community's efforts.
  - **Difficulty Retarget**: The network adjusts its difficulty every 2304 blocks, ensuring consistent block times and network stability, even as mining power fluctuates.

## Community-Driven Development

Bitrae is a testament to the power of community-driven development. Created with no pre-mine, every Bitrae coin in circulation is the result of the hard work and dedication of our miners. This approach guarantees a fair start and equitable opportunity for all participants, reinforcing our vision of a decentralized and accessible cryptocurrency.

## Getting Started with Bitrae

### Installing a Full Node

To set up a full node and join the Bitrae network, please use the `Bitrae_Node_Installer.sh` script available in our repository. This script simplifies the installation process, especially for users without prior experience, and is configured to install the node with non-GUI options by default.

**Installation Steps:**

1. Download the `Bitrae_Node_Installer.sh` script from our repository with the following command:
    ```sh
    wget https://raw.githubusercontent.com/bitraenet/bitrae/main/Bitrae_Node_Installer.sh
    ```
2. Make the script executable with the command:
    ```sh
    chmod +x Bitrae_Node_Installer.sh
    ```
3. Run the script with the command:
    ```sh
    bash Bitrae_Node_Installer.sh
    ```
4. Follow the on-screen instructions to complete the installation.



**Note**: By default, the script installs the node without a GUI. If you prefer a GUI version, you need to edit line 80 of the script. Remove the `--without-gui` flag from this line:

```
./configure BDB_LIBS="-L${BDB_PREFIX}/lib -ldb_cxx-4.8" BDB_CFLAGS="-I${BDB_PREFIX}/include" --without-gui
```

Feel free to modify other options in the script according to your preferences.

For more information and to download the client software, please visit https://bitrae.net

## License

Bitrae is released under the terms of the MIT license. See `COPYING` for more information or see https://opensource.org/licenses/MIT

## Development Workflow

### Overview

Our development process encourages collaboration and innovation. Developers initiate their work in personal branches, progressing to pull requests when they believe their enhancements or fixes are prepared for integration.

### Pull Requests and Review Process

- **Straightforward Changes**: For minor, clear-cut modifications, a member of the Bitrae development team will merge the changes directly.
  
- **Complex or Debatable Changes**: Proposals that introduce complexity or might stir debate require the contributor to engage with both the development team and the wider community. This step ensures that all perspectives are considered, fostering a collaborative decision-making process.

- **Consensus and Contribution**: Acceptance of changes

 hinges on a consensus within the community that the contribution is beneficial. Contributors should be prepared to refine their submissions in response to feedback, ensuring alignment with the project’s coding standards (refer to `doc/coding.txt` for guidelines) and the overarching vision.

### Stability and Releases

- **Main Branch**: The `main` branch undergoes regular builds and tests to ensure ongoing quality and functionality. However, complete stability of this branch is not assured.
  
- **Stable Releases**: For stable and official releases of Bitrae, we use [tags](https://github.com/bitraenet/bitrae/tags) to mark these milestones. These tagged versions represent the culmination of collaborative efforts toward enhancing and refining Bitrae.

## Quality Assurance and Contribution Process

### Testing and Code Review: Critical Pathways for Development

In the realm of development, testing and code review represent our most significant checkpoints. The volume of pull requests frequently exceeds our capacity for review and thorough testing. We request your understanding and patience during this process.

### Collaboration and Vigilance

As a project with critical implications for security, every contribution carries the weight of potential financial impact. Missteps, however minor, can lead to substantial monetary losses for users. We, therefore, emphasize the importance of meticulousness in our development efforts.

### Engage and Support

Your participation and support are invaluable to us. Engaging in testing, contributing to code reviews, or simply offering insights can greatly accelerate the development process. Together, we can maintain the high standards of security and reliability that our community expects.

We are deeply grateful for your contributions and patience, reminding ourselves of the profound responsibility we share in this security-sensitive endeavor.

### Automated Testing

We highly encourage developers to incorporate unit testing as a fundamental part of the development process. This includes writing new unit tests for freshly developed code as well as enhancing existing code with additional unit tests.

For unit testing of the core code, refer to `src/test/`. To compile and run them:

    cd src; make -f makefile.unix test

Unit tests for the GUI code are in `src/qt/test/`. To compile and run them:

    qmake BITCOIN_QT_TEST=1 -o Makefile.test bitcoin-qt.pro
    make -f Makefile.test
    ./bitrae-qt_test

---
