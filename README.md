<h1 align="center"> CariocaFlow: Data Stream Dynamics </h1>

<hr>

<h3 align="center"> Project A1: Modeling a Data Pipeline Development Framework </h3>
<h3 align="center"> Scalar Computing Discipline </h3>

<hr>

<h2> Group Members: </h2>
<ul>
    <li>Dávila Meireles</li>
    <li>Dominique de Vargas de Azevedo</li>
    <li>Lívia Verly</li>
    <li>Pedro Thomaz Conzatti Martins</li>
    <li>Thiago Franke Melchiors</li>
</ul>

<hr>

<p align="justify"> 
    Welcome to <b>CariocaFlow</b>, a framework inspired by the vibrant rhythms and flows of Rio de Janeiro, tailored for building and managing robust data pipelines in the field of data science. 
</p>

<h2> Getting Started </h2> 

<p align="justify">
    Before diving into the waves of data with CariocaFlow, ensure you have a complete download of this repository. The ocean is vast, but you'll navigate it smoothly with the right tools at hand.
</p>

<h2> Prerequisites </h2> 

<p align="justify"> 
    Ensure that your system is prepared with the following essential tools:
</p>

<ul>
    <li><b>Python Environment</b>: A Python environment is crucial for generating simulated data. If Python is not yet installed, please visit <a href="https://python.org">python.org</a> for guidance on setting up Python on your machine.</li>
    <li><b>C++ Compiler</b>: It's vital to have a C++ compiler installed to build and run the framework. For different operating systems, follow these instructions:</li>
        <ul>
            <li><b>Linux</b>: Most Linux distributions come with GCC pre-installed. If not, you can typically install it using your distribution's package manager. For example, on Ubuntu, you can run <code>sudo apt-get install build-essential</code>.</li>
            <li><b>macOS</b>: The Clang compiler comes with Xcode, which can be installed from the App Store. Alternatively, you can install just the command line tools using <code>xcode-select --install</code>.</li>
            <li><b>Windows</b>: You can use Microsoft's Visual Studio Community Edition, which comes with the MSVC compiler. It is available for free download at <a href="https://visualstudio.microsoft.com/">Visual Studio</a>. Another option is to use MinGW or Cygwin for GCC on Windows.</li>
            <li><b>Windows Subsystem for Linux (WSL)</b>: If you prefer a Linux-like environment on Windows, WSL with the Ubuntu distribution is a great choice. Follow the official Microsoft guide <a href="https://docs.microsoft.com/en-us/windows/wsl/install">here</a> for installation instructions.</li>
        </ul>
</ul>

<p align="justify"> 
    With Python and a C++ compiler ready, you're all set to navigate through the data streams with CariocaFlow.
</p>

<hr>

<h2> Generating Mock Data </h2> 

<p align="justify">
    CariocaFlow is equipped with data mock scripts to mirror real-world data sources and test the framework's performance:
</p>

<ul>
    <li><b>DataCat Mock</b>: Emulates a log extraction service</li>
    <li><b>ContaVerde Mock</b>: Simulates an ERP system for management support</li>
    <li><b>CadêAnalytics Mock</b>: Imitates a user behavior monitoring service</li>
</ul>

<p align="justify">
    Follow these steps to generate mock data:
</p>

<ol>
    <li>Navigate into the <code>DataMocks</code> directory.</li>
    <li>Execute the <code>DataCat_Mock.py</code>, <code>ContaVerde_Mock.py</code>, and <code>CadeAnalytics_Mock.py</code> scripts individually. These scripts will conjure up the data for DataCat, ContaVerde, and CadêAnalytics services, respectively.</li>
</ol>

<p align="justify">
    The mock scripts require the following Python libraries: <code>typing</code>, <code>datetime</code>, <code>random</code>, <code>json</code>, <code>csv</code>, and <code>dateutil.relativedelta</code>. You can install them by playing the following rhythm in your terminal:
</p>

        pip install typing datetime random json csv python-dateutil

<h2> Running the Framework </h2>

<p> To execute the framework: </p>
<ol>
    <li> Set up your data repositories, handlers, and triggers as desired, and add them to the pipeline within the Pipeline class. </li>
    <li> Compile and run the provided example <code>main.cpp</code> using the commands below in your computer's terminal. Ensure you replace "<code>/path/to/the/directory</code>" with the actual path to your source directory.
            <pre><code>cd "/path/to/the/directory/" && g++ -o main main.cpp Dataframe.cpp Handler.cpp Queue.cpp Trigger.cpp && ./main</code></pre>
    </li>
</ol>

<p align="justify">
    The framework utilizes several standard C++ libraries to facilitate its operations. These include <code>&lt;stdexcept&gt;</code> for exception handling, <code>&lt;vector&gt;</code> and <code>&lt;string&gt;</code> for data storage and manipulation, <code>&lt;functional&gt;</code> for function objects, <code>&lt;variant&gt;</code> for type-safe unions, <code>&lt;mutex&gt;</code>, <code>&lt;limits&gt;</code>, <code>&lt;condition_variable&gt;</code>, and <code>&lt;chrono&gt;</code> for concurrency and timing, as well as <code>&lt;thread&gt;</code> for multi-threading capabilities. All these libraries are part of the standard library included with any compliant C++ compiler, ensuring that no additional installations are necessary to work with the basic version of C++.
</p>

