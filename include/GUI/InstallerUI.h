#pragma once

#include <string>
#include <vector>
#include <functional>

/**
 * InstallerUI class provides a graphical user interface for the installation process
 * It guides users through steps like license agreement, component selection, and installation
 */
class InstallerUI {
public:
    InstallerUI();
    ~InstallerUI();

    // Initialize the installer UI
    bool initialize();
    
    // Run the installer process
    int run();
    
    // Set installation path
    void setInstallationPath(const std::string& path);
    
    // Set available components for installation
    void setComponents(const std::vector<std::string>& componentNames, 
                       const std::vector<std::string>& descriptions,
                       const std::vector<bool>& defaultSelection);
    
    // Callbacks for installation events
    void setOnInstallStartCallback(std::function<void()> callback);
    void setOnInstallCompleteCallback(std::function<void(bool)> callback);
    void setOnComponentToggleCallback(std::function<void(const std::string&, bool)> callback);
    
    // Set license text
    void setLicenseText(const std::string& text);
    
    // Update installation progress
    void updateProgress(float progress, const std::string& currentTask);

private:
    enum class InstallStep {
        Welcome,
        License,
        Components,
        Location,
        Confirm,
        Installing,
        Complete
    };

    // Internal state
    InstallStep currentStep;
    std::string installPath;
    std::string licenseText;
    std::vector<std::string> componentNames;
    std::vector<std::string> componentDescriptions;
    std::vector<bool> componentSelection;
    float installProgress;
    std::string currentTask;
    bool installationSuccessful;
    
    // Callbacks
    std::function<void()> onInstallStart;
    std::function<void(bool)> onInstallComplete;
    std::function<void(const std::string&, bool)> onComponentToggle;
    
    // Internal methods for UI rendering
    void renderWelcomeScreen();
    void renderLicenseScreen();
    void renderComponentSelection();
    void renderLocationSelection();
    void renderConfirmation();
    void renderInstallProgress();
    void renderCompleteScreen();
    
    // Navigation between steps
    void nextStep();
    void previousStep();
    
    // Perform the installation
    void startInstallation();
};
