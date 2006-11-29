namespace SkeletonVisualizer
{
    partial class FormTopologyHunter
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.openFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.tabControl = new System.Windows.Forms.TabControl();
            this.tabPageDataSources = new System.Windows.Forms.TabPage();
            this.buttonBrowseSequence = new System.Windows.Forms.Button();
            this.buttonBrowseSkeleton = new System.Windows.Forms.Button();
            this.buttonBrowseSheetLocations = new System.Windows.Forms.Button();
            this.buttonBrowseHelixLocations = new System.Windows.Forms.Button();
            this.buttonBrowseHelixLengths = new System.Windows.Forms.Button();
            this.textBoxSequence = new System.Windows.Forms.TextBox();
            this.labelSequence = new System.Windows.Forms.Label();
            this.textBoxSkeleton = new System.Windows.Forms.TextBox();
            this.labelSkeleton = new System.Windows.Forms.Label();
            this.textBoxSheetLocations = new System.Windows.Forms.TextBox();
            this.labelSheetLocations = new System.Windows.Forms.Label();
            this.textBoxHelixLocations = new System.Windows.Forms.TextBox();
            this.labelHelixLocations = new System.Windows.Forms.Label();
            this.textBoxHelixLengths = new System.Windows.Forms.TextBox();
            this.labelHelixLengths = new System.Windows.Forms.Label();
            this.tabPageStandardSettings = new System.Windows.Forms.TabPage();
            this.textBoxMissingSheetCount = new System.Windows.Forms.TextBox();
            this.textBoxMissingHelixCount = new System.Windows.Forms.TextBox();
            this.checkBoxMissingSheetCount = new System.Windows.Forms.CheckBox();
            this.checkBoxMissingHelixCount = new System.Windows.Forms.CheckBox();
            this.checkBoxRepositionSkeleton = new System.Windows.Forms.CheckBox();
            this.labelVoxelSize = new System.Windows.Forms.Label();
            this.textBoxVoxelSize = new System.Windows.Forms.TextBox();
            this.groupBoxCostFunction = new System.Windows.Forms.GroupBox();
            this.radioButtonQuadraticError = new System.Windows.Forms.RadioButton();
            this.radioButtonNormalizedDifference = new System.Windows.Forms.RadioButton();
            this.radioButtonAbsoluteDifference = new System.Windows.Forms.RadioButton();
            this.tabPageAdvancedSettings = new System.Windows.Forms.TabPage();
            this.labelBorderMarginThreshold = new System.Windows.Forms.Label();
            this.textBoxBorderMarginThreshold = new System.Windows.Forms.TextBox();
            this.labelEuclideanPDBRatio = new System.Windows.Forms.Label();
            this.textBoxEuclideanPDBRatio = new System.Windows.Forms.TextBox();
            this.groupBoxCoefficients = new System.Windows.Forms.GroupBox();
            this.textBoxSheetWeightCoeff = new System.Windows.Forms.TextBox();
            this.labelSheetWeightCoeff = new System.Windows.Forms.Label();
            this.textBoxLoopWeightCoeff = new System.Windows.Forms.TextBox();
            this.labelLoopWeightCoeff = new System.Windows.Forms.Label();
            this.textBoxHelixWeightCoeff = new System.Windows.Forms.TextBox();
            this.labelHelixWeightCoeff = new System.Windows.Forms.Label();
            this.groupBoxPenalties = new System.Windows.Forms.GroupBox();
            this.labelMissingHelixPenalty = new System.Windows.Forms.Label();
            this.textBoxMissingHelixPenalty = new System.Windows.Forms.TextBox();
            this.textBoxStartEndMissingHelixPenalty = new System.Windows.Forms.TextBox();
            this.labelEuclideanLoopPenalty = new System.Windows.Forms.Label();
            this.labelStartEndMissingHelixPenalty = new System.Windows.Forms.Label();
            this.textBoxEuclideanLoopPenalty = new System.Windows.Forms.TextBox();
            this.labelMissingHelixLength = new System.Windows.Forms.Label();
            this.textBoxMissingHelixLength = new System.Windows.Forms.TextBox();
            this.toolTip = new System.Windows.Forms.ToolTip(this.components);
            this.buttonReset = new System.Windows.Forms.Button();
            this.buttonStart = new System.Windows.Forms.Button();
            this.labelMaxEuclideanLoopLength = new System.Windows.Forms.Label();
            this.textBoxMaxEuclideanLoopLength = new System.Windows.Forms.TextBox();
            this.tabPageOutput = new System.Windows.Forms.TabPage();
            this.richTextBoxOutput = new System.Windows.Forms.RichTextBox();
            this.tabControl.SuspendLayout();
            this.tabPageDataSources.SuspendLayout();
            this.tabPageStandardSettings.SuspendLayout();
            this.groupBoxCostFunction.SuspendLayout();
            this.tabPageAdvancedSettings.SuspendLayout();
            this.groupBoxCoefficients.SuspendLayout();
            this.groupBoxPenalties.SuspendLayout();
            this.tabPageOutput.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabControl
            // 
            this.tabControl.Controls.Add(this.tabPageDataSources);
            this.tabControl.Controls.Add(this.tabPageStandardSettings);
            this.tabControl.Controls.Add(this.tabPageAdvancedSettings);
            this.tabControl.Controls.Add(this.tabPageOutput);
            this.tabControl.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl.Location = new System.Drawing.Point(0, 0);
            this.tabControl.MinimumSize = new System.Drawing.Size(330, 342);
            this.tabControl.Name = "tabControl";
            this.tabControl.SelectedIndex = 0;
            this.tabControl.Size = new System.Drawing.Size(603, 347);
            this.tabControl.TabIndex = 0;
            // 
            // tabPageDataSources
            // 
            this.tabPageDataSources.Controls.Add(this.buttonBrowseSequence);
            this.tabPageDataSources.Controls.Add(this.buttonBrowseSkeleton);
            this.tabPageDataSources.Controls.Add(this.buttonBrowseSheetLocations);
            this.tabPageDataSources.Controls.Add(this.buttonBrowseHelixLocations);
            this.tabPageDataSources.Controls.Add(this.buttonBrowseHelixLengths);
            this.tabPageDataSources.Controls.Add(this.textBoxSequence);
            this.tabPageDataSources.Controls.Add(this.labelSequence);
            this.tabPageDataSources.Controls.Add(this.textBoxSkeleton);
            this.tabPageDataSources.Controls.Add(this.labelSkeleton);
            this.tabPageDataSources.Controls.Add(this.textBoxSheetLocations);
            this.tabPageDataSources.Controls.Add(this.labelSheetLocations);
            this.tabPageDataSources.Controls.Add(this.textBoxHelixLocations);
            this.tabPageDataSources.Controls.Add(this.labelHelixLocations);
            this.tabPageDataSources.Controls.Add(this.textBoxHelixLengths);
            this.tabPageDataSources.Controls.Add(this.labelHelixLengths);
            this.tabPageDataSources.Location = new System.Drawing.Point(4, 22);
            this.tabPageDataSources.Name = "tabPageDataSources";
            this.tabPageDataSources.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageDataSources.Size = new System.Drawing.Size(595, 321);
            this.tabPageDataSources.TabIndex = 0;
            this.tabPageDataSources.Text = "Data Sources";
            this.tabPageDataSources.UseVisualStyleBackColor = true;
            // 
            // buttonBrowseSequence
            // 
            this.buttonBrowseSequence.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonBrowseSequence.Location = new System.Drawing.Point(553, 120);
            this.buttonBrowseSequence.Name = "buttonBrowseSequence";
            this.buttonBrowseSequence.Size = new System.Drawing.Size(25, 20);
            this.buttonBrowseSequence.TabIndex = 16;
            this.buttonBrowseSequence.Text = "...";
            this.toolTip.SetToolTip(this.buttonBrowseSequence, "The file containing the protein sequence (*.pdb)");
            this.buttonBrowseSequence.UseVisualStyleBackColor = true;
            this.buttonBrowseSequence.Click += new System.EventHandler(this.buttonBrowseSequence_Click);
            // 
            // buttonBrowseSkeleton
            // 
            this.buttonBrowseSkeleton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonBrowseSkeleton.Location = new System.Drawing.Point(553, 94);
            this.buttonBrowseSkeleton.Name = "buttonBrowseSkeleton";
            this.buttonBrowseSkeleton.Size = new System.Drawing.Size(25, 20);
            this.buttonBrowseSkeleton.TabIndex = 15;
            this.buttonBrowseSkeleton.Text = "...";
            this.toolTip.SetToolTip(this.buttonBrowseSkeleton, "The file containing the Cryo-EM skeleton (*.mrc)");
            this.buttonBrowseSkeleton.UseVisualStyleBackColor = true;
            this.buttonBrowseSkeleton.Click += new System.EventHandler(this.buttonBrowseSkeleton_Click);
            // 
            // buttonBrowseSheetLocations
            // 
            this.buttonBrowseSheetLocations.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonBrowseSheetLocations.Location = new System.Drawing.Point(553, 68);
            this.buttonBrowseSheetLocations.Name = "buttonBrowseSheetLocations";
            this.buttonBrowseSheetLocations.Size = new System.Drawing.Size(25, 20);
            this.buttonBrowseSheetLocations.TabIndex = 14;
            this.buttonBrowseSheetLocations.Text = "...";
            this.toolTip.SetToolTip(this.buttonBrowseSheetLocations, "The file containing the predicted locations of the sheets (*.wrl)");
            this.buttonBrowseSheetLocations.UseVisualStyleBackColor = true;
            this.buttonBrowseSheetLocations.Click += new System.EventHandler(this.buttonBrowseSheetLocations_Click);
            // 
            // buttonBrowseHelixLocations
            // 
            this.buttonBrowseHelixLocations.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonBrowseHelixLocations.Location = new System.Drawing.Point(553, 42);
            this.buttonBrowseHelixLocations.Name = "buttonBrowseHelixLocations";
            this.buttonBrowseHelixLocations.Size = new System.Drawing.Size(25, 20);
            this.buttonBrowseHelixLocations.TabIndex = 13;
            this.buttonBrowseHelixLocations.Text = "...";
            this.toolTip.SetToolTip(this.buttonBrowseHelixLocations, "The file containing the predicted locations of the helixes (*.wrl)");
            this.buttonBrowseHelixLocations.UseVisualStyleBackColor = true;
            this.buttonBrowseHelixLocations.Click += new System.EventHandler(this.buttonBrowseHelixLocations_Click);
            // 
            // buttonBrowseHelixLengths
            // 
            this.buttonBrowseHelixLengths.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonBrowseHelixLengths.Location = new System.Drawing.Point(553, 16);
            this.buttonBrowseHelixLengths.Name = "buttonBrowseHelixLengths";
            this.buttonBrowseHelixLengths.Size = new System.Drawing.Size(25, 20);
            this.buttonBrowseHelixLengths.TabIndex = 12;
            this.buttonBrowseHelixLengths.Text = "...";
            this.toolTip.SetToolTip(this.buttonBrowseHelixLengths, "The file containing the predicted lengths of the helixes. (*.SSE)");
            this.buttonBrowseHelixLengths.UseVisualStyleBackColor = true;
            this.buttonBrowseHelixLengths.Click += new System.EventHandler(this.buttonBrowseHelixLengths_Click);
            // 
            // textBoxSequence
            // 
            this.textBoxSequence.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxSequence.BackColor = System.Drawing.Color.White;
            this.textBoxSequence.Location = new System.Drawing.Point(118, 120);
            this.textBoxSequence.Name = "textBoxSequence";
            this.textBoxSequence.ReadOnly = true;
            this.textBoxSequence.Size = new System.Drawing.Size(429, 20);
            this.textBoxSequence.TabIndex = 11;
            this.toolTip.SetToolTip(this.textBoxSequence, "The file containing the protein sequence (*.pdb)");
            // 
            // labelSequence
            // 
            this.labelSequence.AutoSize = true;
            this.labelSequence.Location = new System.Drawing.Point(8, 127);
            this.labelSequence.Name = "labelSequence";
            this.labelSequence.Size = new System.Drawing.Size(62, 13);
            this.labelSequence.TabIndex = 10;
            this.labelSequence.Text = "Sequence :";
            this.toolTip.SetToolTip(this.labelSequence, "The file containing the protein sequence (*.pdb)");
            // 
            // textBoxSkeleton
            // 
            this.textBoxSkeleton.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxSkeleton.BackColor = System.Drawing.Color.White;
            this.textBoxSkeleton.Location = new System.Drawing.Point(118, 94);
            this.textBoxSkeleton.Name = "textBoxSkeleton";
            this.textBoxSkeleton.ReadOnly = true;
            this.textBoxSkeleton.Size = new System.Drawing.Size(429, 20);
            this.textBoxSkeleton.TabIndex = 9;
            this.toolTip.SetToolTip(this.textBoxSkeleton, "The file containing the Cryo-EM skeleton (*.mrc)");
            // 
            // labelSkeleton
            // 
            this.labelSkeleton.AutoSize = true;
            this.labelSkeleton.Location = new System.Drawing.Point(8, 101);
            this.labelSkeleton.Name = "labelSkeleton";
            this.labelSkeleton.Size = new System.Drawing.Size(95, 13);
            this.labelSkeleton.TabIndex = 8;
            this.labelSkeleton.Text = "Cryo-EM Skeleton:";
            this.toolTip.SetToolTip(this.labelSkeleton, "The file containing the Cryo-EM skeleton (*.mrc)");
            // 
            // textBoxSheetLocations
            // 
            this.textBoxSheetLocations.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxSheetLocations.BackColor = System.Drawing.Color.White;
            this.textBoxSheetLocations.Location = new System.Drawing.Point(118, 68);
            this.textBoxSheetLocations.Name = "textBoxSheetLocations";
            this.textBoxSheetLocations.ReadOnly = true;
            this.textBoxSheetLocations.Size = new System.Drawing.Size(429, 20);
            this.textBoxSheetLocations.TabIndex = 7;
            this.toolTip.SetToolTip(this.textBoxSheetLocations, "The file containing the predicted locations of the sheets (*.wrl)");
            // 
            // labelSheetLocations
            // 
            this.labelSheetLocations.AutoSize = true;
            this.labelSheetLocations.Location = new System.Drawing.Point(8, 75);
            this.labelSheetLocations.Name = "labelSheetLocations";
            this.labelSheetLocations.Size = new System.Drawing.Size(104, 13);
            this.labelSheetLocations.TabIndex = 6;
            this.labelSheetLocations.Text = "3D Sheet Locations:";
            this.toolTip.SetToolTip(this.labelSheetLocations, "The file containing the predicted locations of the sheets (*.wrl)");
            // 
            // textBoxHelixLocations
            // 
            this.textBoxHelixLocations.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxHelixLocations.BackColor = System.Drawing.Color.White;
            this.textBoxHelixLocations.Location = new System.Drawing.Point(118, 42);
            this.textBoxHelixLocations.Name = "textBoxHelixLocations";
            this.textBoxHelixLocations.ReadOnly = true;
            this.textBoxHelixLocations.Size = new System.Drawing.Size(429, 20);
            this.textBoxHelixLocations.TabIndex = 5;
            this.toolTip.SetToolTip(this.textBoxHelixLocations, "The file containing the predicted locations of the helixes (*.wrl)");
            // 
            // labelHelixLocations
            // 
            this.labelHelixLocations.AutoSize = true;
            this.labelHelixLocations.Location = new System.Drawing.Point(8, 49);
            this.labelHelixLocations.Name = "labelHelixLocations";
            this.labelHelixLocations.Size = new System.Drawing.Size(99, 13);
            this.labelHelixLocations.TabIndex = 4;
            this.labelHelixLocations.Text = "3D Helix Locations:";
            this.toolTip.SetToolTip(this.labelHelixLocations, "The file containing the predicted locations of the helixes (*.wrl)");
            // 
            // textBoxHelixLengths
            // 
            this.textBoxHelixLengths.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxHelixLengths.BackColor = System.Drawing.Color.White;
            this.textBoxHelixLengths.Location = new System.Drawing.Point(118, 16);
            this.textBoxHelixLengths.Name = "textBoxHelixLengths";
            this.textBoxHelixLengths.ReadOnly = true;
            this.textBoxHelixLengths.Size = new System.Drawing.Size(429, 20);
            this.textBoxHelixLengths.TabIndex = 3;
            this.toolTip.SetToolTip(this.textBoxHelixLengths, "The file containing the predicted lengths of the helixes. (*.SSE)");
            // 
            // labelHelixLengths
            // 
            this.labelHelixLengths.AutoSize = true;
            this.labelHelixLengths.Location = new System.Drawing.Point(8, 23);
            this.labelHelixLengths.Name = "labelHelixLengths";
            this.labelHelixLengths.Size = new System.Drawing.Size(74, 13);
            this.labelHelixLengths.TabIndex = 2;
            this.labelHelixLengths.Text = "Helix Lengths:";
            this.toolTip.SetToolTip(this.labelHelixLengths, "The file containing the predicted lengths of the helixes. (*.SSE)");
            // 
            // tabPageStandardSettings
            // 
            this.tabPageStandardSettings.Controls.Add(this.labelMaxEuclideanLoopLength);
            this.tabPageStandardSettings.Controls.Add(this.textBoxMaxEuclideanLoopLength);
            this.tabPageStandardSettings.Controls.Add(this.textBoxMissingSheetCount);
            this.tabPageStandardSettings.Controls.Add(this.textBoxMissingHelixCount);
            this.tabPageStandardSettings.Controls.Add(this.checkBoxMissingSheetCount);
            this.tabPageStandardSettings.Controls.Add(this.checkBoxMissingHelixCount);
            this.tabPageStandardSettings.Controls.Add(this.checkBoxRepositionSkeleton);
            this.tabPageStandardSettings.Controls.Add(this.labelVoxelSize);
            this.tabPageStandardSettings.Controls.Add(this.textBoxVoxelSize);
            this.tabPageStandardSettings.Controls.Add(this.groupBoxCostFunction);
            this.tabPageStandardSettings.Location = new System.Drawing.Point(4, 22);
            this.tabPageStandardSettings.Name = "tabPageStandardSettings";
            this.tabPageStandardSettings.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageStandardSettings.Size = new System.Drawing.Size(547, 321);
            this.tabPageStandardSettings.TabIndex = 1;
            this.tabPageStandardSettings.Text = "Standard Settings";
            this.tabPageStandardSettings.UseVisualStyleBackColor = true;
            // 
            // textBoxMissingSheetCount
            // 
            this.textBoxMissingSheetCount.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxMissingSheetCount.Enabled = false;
            this.textBoxMissingSheetCount.Location = new System.Drawing.Point(178, 197);
            this.textBoxMissingSheetCount.Name = "textBoxMissingSheetCount";
            this.textBoxMissingSheetCount.Size = new System.Drawing.Size(362, 20);
            this.textBoxMissingSheetCount.TabIndex = 7;
            this.toolTip.SetToolTip(this.textBoxMissingSheetCount, "The number of missing sheets... (Leave unchecked for automatic detection)");
            // 
            // textBoxMissingHelixCount
            // 
            this.textBoxMissingHelixCount.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxMissingHelixCount.Enabled = false;
            this.textBoxMissingHelixCount.Location = new System.Drawing.Point(178, 174);
            this.textBoxMissingHelixCount.Name = "textBoxMissingHelixCount";
            this.textBoxMissingHelixCount.Size = new System.Drawing.Size(362, 20);
            this.textBoxMissingHelixCount.TabIndex = 6;
            this.toolTip.SetToolTip(this.textBoxMissingHelixCount, "The number of missing helixes... (Leave unchecked for automatic detection)");
            // 
            // checkBoxMissingSheetCount
            // 
            this.checkBoxMissingSheetCount.AutoSize = true;
            this.checkBoxMissingSheetCount.Location = new System.Drawing.Point(11, 200);
            this.checkBoxMissingSheetCount.Name = "checkBoxMissingSheetCount";
            this.checkBoxMissingSheetCount.Size = new System.Drawing.Size(129, 17);
            this.checkBoxMissingSheetCount.TabIndex = 5;
            this.checkBoxMissingSheetCount.Text = "No of Missing Sheets:";
            this.toolTip.SetToolTip(this.checkBoxMissingSheetCount, "The number of missing sheets... (Leave unchecked for automatic detection)");
            this.checkBoxMissingSheetCount.UseVisualStyleBackColor = true;
            this.checkBoxMissingSheetCount.CheckedChanged += new System.EventHandler(this.checkBoxMissingSheetCount_CheckedChanged);
            // 
            // checkBoxMissingHelixCount
            // 
            this.checkBoxMissingHelixCount.AutoSize = true;
            this.checkBoxMissingHelixCount.Location = new System.Drawing.Point(11, 177);
            this.checkBoxMissingHelixCount.Name = "checkBoxMissingHelixCount";
            this.checkBoxMissingHelixCount.Size = new System.Drawing.Size(130, 17);
            this.checkBoxMissingHelixCount.TabIndex = 4;
            this.checkBoxMissingHelixCount.Text = "No of Missing Helixes:";
            this.toolTip.SetToolTip(this.checkBoxMissingHelixCount, "The number of missing helixes... (Leave unchecked for automatic detection)");
            this.checkBoxMissingHelixCount.UseVisualStyleBackColor = true;
            this.checkBoxMissingHelixCount.CheckedChanged += new System.EventHandler(this.checkBoxMissingHelixCount_CheckedChanged);
            // 
            // checkBoxRepositionSkeleton
            // 
            this.checkBoxRepositionSkeleton.AutoSize = true;
            this.checkBoxRepositionSkeleton.Location = new System.Drawing.Point(11, 223);
            this.checkBoxRepositionSkeleton.Name = "checkBoxRepositionSkeleton";
            this.checkBoxRepositionSkeleton.Size = new System.Drawing.Size(121, 17);
            this.checkBoxRepositionSkeleton.TabIndex = 3;
            this.checkBoxRepositionSkeleton.Text = "Reposition Skeleton";
            this.toolTip.SetToolTip(this.checkBoxRepositionSkeleton, "Check this box if the Skeleton and the Helix Locations are in two different coord" +
                    "inate systems");
            this.checkBoxRepositionSkeleton.UseVisualStyleBackColor = true;
            // 
            // labelVoxelSize
            // 
            this.labelVoxelSize.AutoSize = true;
            this.labelVoxelSize.Location = new System.Drawing.Point(29, 129);
            this.labelVoxelSize.Name = "labelVoxelSize";
            this.labelVoxelSize.Size = new System.Drawing.Size(59, 13);
            this.labelVoxelSize.TabIndex = 2;
            this.labelVoxelSize.Text = "Voxel Size:";
            this.toolTip.SetToolTip(this.labelVoxelSize, "The size of a voxel (The scale needed to bring the skeleton and helix locations t" +
                    "o the same coordinate system)");
            // 
            // textBoxVoxelSize
            // 
            this.textBoxVoxelSize.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxVoxelSize.Location = new System.Drawing.Point(178, 122);
            this.textBoxVoxelSize.Name = "textBoxVoxelSize";
            this.textBoxVoxelSize.Size = new System.Drawing.Size(362, 20);
            this.textBoxVoxelSize.TabIndex = 1;
            this.textBoxVoxelSize.Text = "1";
            this.toolTip.SetToolTip(this.textBoxVoxelSize, "The size of a voxel (The scale needed to bring the skeleton and helix locations t" +
                    "o the same coordinate system)");
            // 
            // groupBoxCostFunction
            // 
            this.groupBoxCostFunction.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBoxCostFunction.Controls.Add(this.radioButtonQuadraticError);
            this.groupBoxCostFunction.Controls.Add(this.radioButtonNormalizedDifference);
            this.groupBoxCostFunction.Controls.Add(this.radioButtonAbsoluteDifference);
            this.groupBoxCostFunction.Location = new System.Drawing.Point(8, 6);
            this.groupBoxCostFunction.Name = "groupBoxCostFunction";
            this.groupBoxCostFunction.Size = new System.Drawing.Size(532, 107);
            this.groupBoxCostFunction.TabIndex = 0;
            this.groupBoxCostFunction.TabStop = false;
            this.groupBoxCostFunction.Text = "Cost Function:";
            this.toolTip.SetToolTip(this.groupBoxCostFunction, "The cost function to be used");
            // 
            // radioButtonQuadraticError
            // 
            this.radioButtonQuadraticError.AutoSize = true;
            this.radioButtonQuadraticError.Location = new System.Drawing.Point(24, 74);
            this.radioButtonQuadraticError.Name = "radioButtonQuadraticError";
            this.radioButtonQuadraticError.Size = new System.Drawing.Size(96, 17);
            this.radioButtonQuadraticError.TabIndex = 2;
            this.radioButtonQuadraticError.Text = "Quadratic Error";
            this.toolTip.SetToolTip(this.radioButtonQuadraticError, "D = (W1 - W2)^2");
            this.radioButtonQuadraticError.UseVisualStyleBackColor = true;
            // 
            // radioButtonNormalizedDifference
            // 
            this.radioButtonNormalizedDifference.AutoSize = true;
            this.radioButtonNormalizedDifference.Location = new System.Drawing.Point(24, 51);
            this.radioButtonNormalizedDifference.Name = "radioButtonNormalizedDifference";
            this.radioButtonNormalizedDifference.Size = new System.Drawing.Size(129, 17);
            this.radioButtonNormalizedDifference.TabIndex = 1;
            this.radioButtonNormalizedDifference.Text = "Normalized Difference";
            this.toolTip.SetToolTip(this.radioButtonNormalizedDifference, "D = |W1 - W2| / (W1 + W2)");
            this.radioButtonNormalizedDifference.UseVisualStyleBackColor = true;
            // 
            // radioButtonAbsoluteDifference
            // 
            this.radioButtonAbsoluteDifference.AutoSize = true;
            this.radioButtonAbsoluteDifference.Checked = true;
            this.radioButtonAbsoluteDifference.Location = new System.Drawing.Point(24, 28);
            this.radioButtonAbsoluteDifference.Name = "radioButtonAbsoluteDifference";
            this.radioButtonAbsoluteDifference.Size = new System.Drawing.Size(118, 17);
            this.radioButtonAbsoluteDifference.TabIndex = 0;
            this.radioButtonAbsoluteDifference.TabStop = true;
            this.radioButtonAbsoluteDifference.Text = "Absolute Difference";
            this.toolTip.SetToolTip(this.radioButtonAbsoluteDifference, "D = |W1 - W2|");
            this.radioButtonAbsoluteDifference.UseVisualStyleBackColor = true;
            // 
            // tabPageAdvancedSettings
            // 
            this.tabPageAdvancedSettings.Controls.Add(this.labelBorderMarginThreshold);
            this.tabPageAdvancedSettings.Controls.Add(this.textBoxBorderMarginThreshold);
            this.tabPageAdvancedSettings.Controls.Add(this.labelEuclideanPDBRatio);
            this.tabPageAdvancedSettings.Controls.Add(this.textBoxEuclideanPDBRatio);
            this.tabPageAdvancedSettings.Controls.Add(this.groupBoxCoefficients);
            this.tabPageAdvancedSettings.Controls.Add(this.groupBoxPenalties);
            this.tabPageAdvancedSettings.Controls.Add(this.labelMissingHelixLength);
            this.tabPageAdvancedSettings.Controls.Add(this.textBoxMissingHelixLength);
            this.tabPageAdvancedSettings.Location = new System.Drawing.Point(4, 22);
            this.tabPageAdvancedSettings.Name = "tabPageAdvancedSettings";
            this.tabPageAdvancedSettings.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageAdvancedSettings.Size = new System.Drawing.Size(547, 321);
            this.tabPageAdvancedSettings.TabIndex = 2;
            this.tabPageAdvancedSettings.Text = "Advanced Settings";
            this.tabPageAdvancedSettings.UseVisualStyleBackColor = true;
            // 
            // labelBorderMarginThreshold
            // 
            this.labelBorderMarginThreshold.AutoSize = true;
            this.labelBorderMarginThreshold.Location = new System.Drawing.Point(14, 285);
            this.labelBorderMarginThreshold.Name = "labelBorderMarginThreshold";
            this.labelBorderMarginThreshold.Size = new System.Drawing.Size(123, 13);
            this.labelBorderMarginThreshold.TabIndex = 18;
            this.labelBorderMarginThreshold.Text = "Border Margin Threshold";
            this.toolTip.SetToolTip(this.labelBorderMarginThreshold, "The maximum distance to the endpoints of a helix");
            this.labelBorderMarginThreshold.Visible = false;
            // 
            // textBoxBorderMarginThreshold
            // 
            this.textBoxBorderMarginThreshold.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxBorderMarginThreshold.Location = new System.Drawing.Point(206, 278);
            this.textBoxBorderMarginThreshold.Name = "textBoxBorderMarginThreshold";
            this.textBoxBorderMarginThreshold.Size = new System.Drawing.Size(321, 20);
            this.textBoxBorderMarginThreshold.TabIndex = 17;
            this.textBoxBorderMarginThreshold.Text = "3";
            this.toolTip.SetToolTip(this.textBoxBorderMarginThreshold, "The maximum distance to the endpoints of a helix");
            this.textBoxBorderMarginThreshold.Visible = false;
            // 
            // labelEuclideanPDBRatio
            // 
            this.labelEuclideanPDBRatio.AutoSize = true;
            this.labelEuclideanPDBRatio.Location = new System.Drawing.Point(14, 259);
            this.labelEuclideanPDBRatio.Name = "labelEuclideanPDBRatio";
            this.labelEuclideanPDBRatio.Size = new System.Drawing.Size(165, 13);
            this.labelEuclideanPDBRatio.TabIndex = 16;
            this.labelEuclideanPDBRatio.Text = "Euclidean to PDB distance Ratio:";
            this.toolTip.SetToolTip(this.labelEuclideanPDBRatio, "The distance ratio between voxels and the sequence");
            this.labelEuclideanPDBRatio.Visible = false;
            // 
            // textBoxEuclideanPDBRatio
            // 
            this.textBoxEuclideanPDBRatio.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxEuclideanPDBRatio.Location = new System.Drawing.Point(206, 252);
            this.textBoxEuclideanPDBRatio.Name = "textBoxEuclideanPDBRatio";
            this.textBoxEuclideanPDBRatio.Size = new System.Drawing.Size(321, 20);
            this.textBoxEuclideanPDBRatio.TabIndex = 15;
            this.textBoxEuclideanPDBRatio.Text = "10.0";
            this.toolTip.SetToolTip(this.textBoxEuclideanPDBRatio, "The distance ratio between voxels and the sequence");
            this.textBoxEuclideanPDBRatio.Visible = false;
            // 
            // groupBoxCoefficients
            // 
            this.groupBoxCoefficients.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBoxCoefficients.Controls.Add(this.textBoxSheetWeightCoeff);
            this.groupBoxCoefficients.Controls.Add(this.labelSheetWeightCoeff);
            this.groupBoxCoefficients.Controls.Add(this.textBoxLoopWeightCoeff);
            this.groupBoxCoefficients.Controls.Add(this.labelLoopWeightCoeff);
            this.groupBoxCoefficients.Controls.Add(this.textBoxHelixWeightCoeff);
            this.groupBoxCoefficients.Controls.Add(this.labelHelixWeightCoeff);
            this.groupBoxCoefficients.Location = new System.Drawing.Point(8, 109);
            this.groupBoxCoefficients.Name = "groupBoxCoefficients";
            this.groupBoxCoefficients.Size = new System.Drawing.Size(532, 103);
            this.groupBoxCoefficients.TabIndex = 14;
            this.groupBoxCoefficients.TabStop = false;
            this.groupBoxCoefficients.Text = "Coefficients:";
            // 
            // textBoxSheetWeightCoeff
            // 
            this.textBoxSheetWeightCoeff.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxSheetWeightCoeff.Location = new System.Drawing.Point(198, 71);
            this.textBoxSheetWeightCoeff.Name = "textBoxSheetWeightCoeff";
            this.textBoxSheetWeightCoeff.Size = new System.Drawing.Size(321, 20);
            this.textBoxSheetWeightCoeff.TabIndex = 15;
            this.textBoxSheetWeightCoeff.Text = "1.0";
            this.toolTip.SetToolTip(this.textBoxSheetWeightCoeff, "The importance of the difference of sheet sizes");
            // 
            // labelSheetWeightCoeff
            // 
            this.labelSheetWeightCoeff.AutoSize = true;
            this.labelSheetWeightCoeff.Location = new System.Drawing.Point(6, 78);
            this.labelSheetWeightCoeff.Name = "labelSheetWeightCoeff";
            this.labelSheetWeightCoeff.Size = new System.Drawing.Size(97, 13);
            this.labelSheetWeightCoeff.TabIndex = 16;
            this.labelSheetWeightCoeff.Text = "Sheet Importance :";
            this.toolTip.SetToolTip(this.labelSheetWeightCoeff, "The importance of the difference of sheet sizes");
            // 
            // textBoxLoopWeightCoeff
            // 
            this.textBoxLoopWeightCoeff.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxLoopWeightCoeff.Location = new System.Drawing.Point(198, 45);
            this.textBoxLoopWeightCoeff.Name = "textBoxLoopWeightCoeff";
            this.textBoxLoopWeightCoeff.Size = new System.Drawing.Size(321, 20);
            this.textBoxLoopWeightCoeff.TabIndex = 13;
            this.textBoxLoopWeightCoeff.Text = "0.2";
            this.toolTip.SetToolTip(this.textBoxLoopWeightCoeff, "The importance of the difference of loop sizes");
            // 
            // labelLoopWeightCoeff
            // 
            this.labelLoopWeightCoeff.AutoSize = true;
            this.labelLoopWeightCoeff.Location = new System.Drawing.Point(6, 52);
            this.labelLoopWeightCoeff.Name = "labelLoopWeightCoeff";
            this.labelLoopWeightCoeff.Size = new System.Drawing.Size(93, 13);
            this.labelLoopWeightCoeff.TabIndex = 14;
            this.labelLoopWeightCoeff.Text = "Loop Importance :";
            this.toolTip.SetToolTip(this.labelLoopWeightCoeff, "The importance of the difference of loop sizes");
            // 
            // textBoxHelixWeightCoeff
            // 
            this.textBoxHelixWeightCoeff.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxHelixWeightCoeff.Location = new System.Drawing.Point(198, 19);
            this.textBoxHelixWeightCoeff.Name = "textBoxHelixWeightCoeff";
            this.textBoxHelixWeightCoeff.Size = new System.Drawing.Size(321, 20);
            this.textBoxHelixWeightCoeff.TabIndex = 11;
            this.textBoxHelixWeightCoeff.Text = "1.0";
            this.toolTip.SetToolTip(this.textBoxHelixWeightCoeff, "The importance of the difference of helix sizes");
            // 
            // labelHelixWeightCoeff
            // 
            this.labelHelixWeightCoeff.AutoSize = true;
            this.labelHelixWeightCoeff.Location = new System.Drawing.Point(6, 26);
            this.labelHelixWeightCoeff.Name = "labelHelixWeightCoeff";
            this.labelHelixWeightCoeff.Size = new System.Drawing.Size(92, 13);
            this.labelHelixWeightCoeff.TabIndex = 12;
            this.labelHelixWeightCoeff.Text = "Helix Importance :";
            this.toolTip.SetToolTip(this.labelHelixWeightCoeff, "The importance of the difference of helix sizes");
            // 
            // groupBoxPenalties
            // 
            this.groupBoxPenalties.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBoxPenalties.Controls.Add(this.labelMissingHelixPenalty);
            this.groupBoxPenalties.Controls.Add(this.textBoxMissingHelixPenalty);
            this.groupBoxPenalties.Controls.Add(this.textBoxStartEndMissingHelixPenalty);
            this.groupBoxPenalties.Controls.Add(this.labelEuclideanLoopPenalty);
            this.groupBoxPenalties.Controls.Add(this.labelStartEndMissingHelixPenalty);
            this.groupBoxPenalties.Controls.Add(this.textBoxEuclideanLoopPenalty);
            this.groupBoxPenalties.Location = new System.Drawing.Point(8, 6);
            this.groupBoxPenalties.Name = "groupBoxPenalties";
            this.groupBoxPenalties.Size = new System.Drawing.Size(532, 97);
            this.groupBoxPenalties.TabIndex = 13;
            this.groupBoxPenalties.TabStop = false;
            this.groupBoxPenalties.Text = "Penalties:";
            // 
            // labelMissingHelixPenalty
            // 
            this.labelMissingHelixPenalty.AutoSize = true;
            this.labelMissingHelixPenalty.Location = new System.Drawing.Point(6, 16);
            this.labelMissingHelixPenalty.Name = "labelMissingHelixPenalty";
            this.labelMissingHelixPenalty.Size = new System.Drawing.Size(129, 13);
            this.labelMissingHelixPenalty.TabIndex = 6;
            this.labelMissingHelixPenalty.Text = "\"Helix is Missing\" Penalty:";
            this.toolTip.SetToolTip(this.labelMissingHelixPenalty, "The penalty to be used if a helix is assumed to be missing");
            // 
            // textBoxMissingHelixPenalty
            // 
            this.textBoxMissingHelixPenalty.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxMissingHelixPenalty.Location = new System.Drawing.Point(198, 9);
            this.textBoxMissingHelixPenalty.Name = "textBoxMissingHelixPenalty";
            this.textBoxMissingHelixPenalty.Size = new System.Drawing.Size(321, 20);
            this.textBoxMissingHelixPenalty.TabIndex = 5;
            this.textBoxMissingHelixPenalty.Text = "5.0";
            this.toolTip.SetToolTip(this.textBoxMissingHelixPenalty, "The penalty to be used if a helix is assumed to be missing");
            // 
            // textBoxStartEndMissingHelixPenalty
            // 
            this.textBoxStartEndMissingHelixPenalty.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxStartEndMissingHelixPenalty.Location = new System.Drawing.Point(198, 35);
            this.textBoxStartEndMissingHelixPenalty.Name = "textBoxStartEndMissingHelixPenalty";
            this.textBoxStartEndMissingHelixPenalty.Size = new System.Drawing.Size(321, 20);
            this.textBoxStartEndMissingHelixPenalty.TabIndex = 7;
            this.textBoxStartEndMissingHelixPenalty.Text = "5.0";
            this.toolTip.SetToolTip(this.textBoxStartEndMissingHelixPenalty, "The additional penalty to be used if the first or last helix in the sequence is a" +
                    "ssumed to be missing");
            // 
            // labelEuclideanLoopPenalty
            // 
            this.labelEuclideanLoopPenalty.AutoSize = true;
            this.labelEuclideanLoopPenalty.Location = new System.Drawing.Point(6, 68);
            this.labelEuclideanLoopPenalty.Name = "labelEuclideanLoopPenalty";
            this.labelEuclideanLoopPenalty.Size = new System.Drawing.Size(160, 13);
            this.labelEuclideanLoopPenalty.TabIndex = 10;
            this.labelEuclideanLoopPenalty.Text = "\"Euclidean Loop Used\" Penalty:";
            this.toolTip.SetToolTip(this.labelEuclideanLoopPenalty, "The penalty to be used if a loop segment is missing, and the euclidean distance i" +
                    "s used instead");
            // 
            // labelStartEndMissingHelixPenalty
            // 
            this.labelStartEndMissingHelixPenalty.AutoSize = true;
            this.labelStartEndMissingHelixPenalty.Location = new System.Drawing.Point(6, 42);
            this.labelStartEndMissingHelixPenalty.Name = "labelStartEndMissingHelixPenalty";
            this.labelStartEndMissingHelixPenalty.Size = new System.Drawing.Size(186, 13);
            this.labelStartEndMissingHelixPenalty.TabIndex = 8;
            this.labelStartEndMissingHelixPenalty.Text = "\"First or Last Helix is Missing\" Penalty:";
            this.toolTip.SetToolTip(this.labelStartEndMissingHelixPenalty, "The additional penalty to be used if the first or last helix in the sequence is a" +
                    "ssumed to be missing");
            // 
            // textBoxEuclideanLoopPenalty
            // 
            this.textBoxEuclideanLoopPenalty.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxEuclideanLoopPenalty.Location = new System.Drawing.Point(198, 61);
            this.textBoxEuclideanLoopPenalty.Name = "textBoxEuclideanLoopPenalty";
            this.textBoxEuclideanLoopPenalty.Size = new System.Drawing.Size(321, 20);
            this.textBoxEuclideanLoopPenalty.TabIndex = 9;
            this.textBoxEuclideanLoopPenalty.Text = "5.0";
            this.toolTip.SetToolTip(this.textBoxEuclideanLoopPenalty, "The penalty to be used if a loop segment is missing, and the euclidean distance i" +
                    "s used instead");
            // 
            // labelMissingHelixLength
            // 
            this.labelMissingHelixLength.AutoSize = true;
            this.labelMissingHelixLength.Location = new System.Drawing.Point(14, 233);
            this.labelMissingHelixLength.Name = "labelMissingHelixLength";
            this.labelMissingHelixLength.Size = new System.Drawing.Size(150, 13);
            this.labelMissingHelixLength.TabIndex = 4;
            this.labelMissingHelixLength.Text = "Average Missing Helix Length:";
            this.toolTip.SetToolTip(this.labelMissingHelixLength, "The average length of a missing helix");
            // 
            // textBoxMissingHelixLength
            // 
            this.textBoxMissingHelixLength.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxMissingHelixLength.Location = new System.Drawing.Point(206, 226);
            this.textBoxMissingHelixLength.Name = "textBoxMissingHelixLength";
            this.textBoxMissingHelixLength.Size = new System.Drawing.Size(321, 20);
            this.textBoxMissingHelixLength.TabIndex = 3;
            this.textBoxMissingHelixLength.Text = "5.0";
            this.toolTip.SetToolTip(this.textBoxMissingHelixLength, "The average length of a missing helix");
            // 
            // buttonReset
            // 
            this.buttonReset.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonReset.Location = new System.Drawing.Point(444, 352);
            this.buttonReset.Name = "buttonReset";
            this.buttonReset.Size = new System.Drawing.Size(75, 23);
            this.buttonReset.TabIndex = 1;
            this.buttonReset.Text = "Reset";
            this.toolTip.SetToolTip(this.buttonReset, "Reset the data");
            this.buttonReset.UseVisualStyleBackColor = true;
            this.buttonReset.Click += new System.EventHandler(this.buttonReset_Click);
            // 
            // buttonStart
            // 
            this.buttonStart.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonStart.Location = new System.Drawing.Point(525, 352);
            this.buttonStart.Name = "buttonStart";
            this.buttonStart.Size = new System.Drawing.Size(75, 23);
            this.buttonStart.TabIndex = 2;
            this.buttonStart.Text = "Start";
            this.toolTip.SetToolTip(this.buttonStart, "Start topology detection");
            this.buttonStart.UseVisualStyleBackColor = true;
            this.buttonStart.Click += new System.EventHandler(this.buttonStart_Click);
            // 
            // labelMaxEuclideanLoopLength
            // 
            this.labelMaxEuclideanLoopLength.AutoSize = true;
            this.labelMaxEuclideanLoopLength.Location = new System.Drawing.Point(29, 155);
            this.labelMaxEuclideanLoopLength.Name = "labelMaxEuclideanLoopLength";
            this.labelMaxEuclideanLoopLength.Size = new System.Drawing.Size(143, 13);
            this.labelMaxEuclideanLoopLength.TabIndex = 9;
            this.labelMaxEuclideanLoopLength.Text = "Max Euclidean Loop Length:";
            this.toolTip.SetToolTip(this.labelMaxEuclideanLoopLength, "The maximum length of a euclidean loop segment");
            // 
            // textBoxMaxEuclideanLoopLength
            // 
            this.textBoxMaxEuclideanLoopLength.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxMaxEuclideanLoopLength.Location = new System.Drawing.Point(178, 148);
            this.textBoxMaxEuclideanLoopLength.Name = "textBoxMaxEuclideanLoopLength";
            this.textBoxMaxEuclideanLoopLength.Size = new System.Drawing.Size(362, 20);
            this.textBoxMaxEuclideanLoopLength.TabIndex = 8;
            this.textBoxMaxEuclideanLoopLength.Text = "0";
            this.toolTip.SetToolTip(this.textBoxMaxEuclideanLoopLength, "The maximum length of a euclidean loop segment");
            // 
            // tabPageOutput
            // 
            this.tabPageOutput.Controls.Add(this.richTextBoxOutput);
            this.tabPageOutput.Location = new System.Drawing.Point(4, 22);
            this.tabPageOutput.Name = "tabPageOutput";
            this.tabPageOutput.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageOutput.Size = new System.Drawing.Size(547, 321);
            this.tabPageOutput.TabIndex = 3;
            this.tabPageOutput.Text = "Output";
            this.tabPageOutput.UseVisualStyleBackColor = true;
            // 
            // richTextBoxOutput
            // 
            this.richTextBoxOutput.BackColor = System.Drawing.Color.White;
            this.richTextBoxOutput.Dock = System.Windows.Forms.DockStyle.Fill;
            this.richTextBoxOutput.Font = new System.Drawing.Font("Courier New", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.richTextBoxOutput.Location = new System.Drawing.Point(3, 3);
            this.richTextBoxOutput.Name = "richTextBoxOutput";
            this.richTextBoxOutput.ReadOnly = true;
            this.richTextBoxOutput.Size = new System.Drawing.Size(541, 315);
            this.richTextBoxOutput.TabIndex = 0;
            this.richTextBoxOutput.Text = "";
            this.richTextBoxOutput.WordWrap = false;
            // 
            // FormTopologyHunter
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(603, 377);
            this.Controls.Add(this.buttonStart);
            this.Controls.Add(this.buttonReset);
            this.Controls.Add(this.tabControl);
            this.MinimumSize = new System.Drawing.Size(330, 404);
            this.Name = "FormTopologyHunter";
            this.Padding = new System.Windows.Forms.Padding(0, 0, 0, 30);
            this.Text = "Topology Hunter";
            this.tabControl.ResumeLayout(false);
            this.tabPageDataSources.ResumeLayout(false);
            this.tabPageDataSources.PerformLayout();
            this.tabPageStandardSettings.ResumeLayout(false);
            this.tabPageStandardSettings.PerformLayout();
            this.groupBoxCostFunction.ResumeLayout(false);
            this.groupBoxCostFunction.PerformLayout();
            this.tabPageAdvancedSettings.ResumeLayout(false);
            this.tabPageAdvancedSettings.PerformLayout();
            this.groupBoxCoefficients.ResumeLayout(false);
            this.groupBoxCoefficients.PerformLayout();
            this.groupBoxPenalties.ResumeLayout(false);
            this.groupBoxPenalties.PerformLayout();
            this.tabPageOutput.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.OpenFileDialog openFileDialog;
        private System.Windows.Forms.TabControl tabControl;
        private System.Windows.Forms.TabPage tabPageDataSources;
        private System.Windows.Forms.TextBox textBoxHelixLocations;
        private System.Windows.Forms.Label labelHelixLocations;
        private System.Windows.Forms.TextBox textBoxHelixLengths;
        private System.Windows.Forms.Label labelHelixLengths;
        private System.Windows.Forms.TabPage tabPageStandardSettings;
        private System.Windows.Forms.Button buttonBrowseHelixLengths;
        private System.Windows.Forms.TextBox textBoxSequence;
        private System.Windows.Forms.Label labelSequence;
        private System.Windows.Forms.TextBox textBoxSkeleton;
        private System.Windows.Forms.Label labelSkeleton;
        private System.Windows.Forms.TextBox textBoxSheetLocations;
        private System.Windows.Forms.Label labelSheetLocations;
        private System.Windows.Forms.Button buttonBrowseSequence;
        private System.Windows.Forms.Button buttonBrowseSkeleton;
        private System.Windows.Forms.Button buttonBrowseSheetLocations;
        private System.Windows.Forms.Button buttonBrowseHelixLocations;
        private System.Windows.Forms.ToolTip toolTip;
        private System.Windows.Forms.GroupBox groupBoxCostFunction;
        private System.Windows.Forms.RadioButton radioButtonQuadraticError;
        private System.Windows.Forms.RadioButton radioButtonNormalizedDifference;
        private System.Windows.Forms.RadioButton radioButtonAbsoluteDifference;
        private System.Windows.Forms.TabPage tabPageAdvancedSettings;
        private System.Windows.Forms.Label labelVoxelSize;
        private System.Windows.Forms.TextBox textBoxVoxelSize;
        private System.Windows.Forms.CheckBox checkBoxRepositionSkeleton;
        private System.Windows.Forms.CheckBox checkBoxMissingSheetCount;
        private System.Windows.Forms.CheckBox checkBoxMissingHelixCount;
        private System.Windows.Forms.TextBox textBoxMissingSheetCount;
        private System.Windows.Forms.TextBox textBoxMissingHelixCount;
        private System.Windows.Forms.Label labelMissingHelixLength;
        private System.Windows.Forms.TextBox textBoxMissingHelixLength;
        private System.Windows.Forms.Label labelEuclideanLoopPenalty;
        private System.Windows.Forms.TextBox textBoxEuclideanLoopPenalty;
        private System.Windows.Forms.Label labelStartEndMissingHelixPenalty;
        private System.Windows.Forms.TextBox textBoxStartEndMissingHelixPenalty;
        private System.Windows.Forms.Label labelMissingHelixPenalty;
        private System.Windows.Forms.TextBox textBoxMissingHelixPenalty;
        private System.Windows.Forms.GroupBox groupBoxCoefficients;
        private System.Windows.Forms.TextBox textBoxSheetWeightCoeff;
        private System.Windows.Forms.Label labelSheetWeightCoeff;
        private System.Windows.Forms.TextBox textBoxLoopWeightCoeff;
        private System.Windows.Forms.Label labelLoopWeightCoeff;
        private System.Windows.Forms.TextBox textBoxHelixWeightCoeff;
        private System.Windows.Forms.Label labelHelixWeightCoeff;
        private System.Windows.Forms.GroupBox groupBoxPenalties;
        private System.Windows.Forms.Label labelEuclideanPDBRatio;
        private System.Windows.Forms.TextBox textBoxEuclideanPDBRatio;
        private System.Windows.Forms.Label labelBorderMarginThreshold;
        private System.Windows.Forms.TextBox textBoxBorderMarginThreshold;
        private System.Windows.Forms.Button buttonReset;
        private System.Windows.Forms.Button buttonStart;
        private System.Windows.Forms.Label labelMaxEuclideanLoopLength;
        private System.Windows.Forms.TextBox textBoxMaxEuclideanLoopLength;
        private System.Windows.Forms.TabPage tabPageOutput;
        private System.Windows.Forms.RichTextBox richTextBoxOutput;
    }
}

