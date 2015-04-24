using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace wustl_mm.SkeletonVisualizer
{
    public partial class FormTopologyHunter : Form
    {
        #region Attributes
        public delegate void SetLogDelegate(string data);
        public delegate void SetArrowCursorDelegate();
        #endregion

        #region Constructor / Destructor
        public FormTopologyHunter()
        {
            InitializeComponent();
            ResetForm(false);
        }
        #endregion

        #region Methods
        private void SelectFile(string title, string filter, TextBox destination)
        {
            openFileDialog.Title = title;
            openFileDialog.Filter = filter;
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                destination.Text = openFileDialog.FileName;
            }
        }
        private void ResetForm(bool confirm)
        {
            if (!confirm || MessageBox.Show("Are you sure you want to reset? \nYou will lose all data.", "Reset Data?", MessageBoxButtons.YesNo, MessageBoxIcon.Warning) == DialogResult.Yes)
            {
                textBoxHelixLengths.Text = string.Empty;
                textBoxHelixLocations.Text = string.Empty;
                textBoxSheetLocations.Text = string.Empty;
                textBoxSkeleton.Text = string.Empty;
                textBoxSequence.Text = string.Empty;
                radioButtonAbsoluteDifference.Checked = true;
                textBoxMaxEuclideanLoopLength.Text = "0";
                textBoxVoxelSize.Text = "1.0";
                checkBoxRepositionSkeleton.Checked = false;
                checkBoxMissingHelixCount.Checked = false;
                textBoxMissingHelixCount.Text = string.Empty;
                checkBoxMissingSheetCount.Checked = false;
                textBoxMissingSheetCount.Text = string.Empty;
                textBoxMissingHelixPenalty.Text = "5.0";
                textBoxStartEndMissingHelixPenalty.Text = "5.0";
                textBoxEuclideanLoopPenalty.Text = "5.0";
                textBoxHelixWeightCoeff.Text = "1.0";
                textBoxLoopWeightCoeff.Text = "0.2";
                textBoxSheetWeightCoeff.Text = "1.0";
                textBoxMissingHelixLength.Text = "5.0";
                textBoxEuclideanPDBRatio.Text = "10.0";
                textBoxBorderMarginThreshold.Text = "3";
            }
        }
        private bool ValidateForm(bool showError)
        {
            bool error = false;
            string errorText = string.Empty;
            double doubleTemp;
            int intTemp;
            if ((textBoxHelixLengths.Text.Length == 0) || !File.Exists(textBoxHelixLengths.Text))
            {
                error = true;
                errorText = "File specifying helix lengths does not exist!";
            }

            if ((textBoxHelixLocations.Text.Length == 0) || !File.Exists(textBoxHelixLocations.Text))
            {
                error = true;
                errorText = "File specifying 3D helix locations does not exist!";
            }

            if ((textBoxSheetLocations.Text.Length == 0) || !File.Exists(textBoxSheetLocations.Text))
            {
                error = true;
                errorText = "File specifying 3D sheet locations does not exist!";
            }

            if ((textBoxSkeleton.Text.Length == 0) || !File.Exists(textBoxSkeleton.Text))
            {
                error = true;
                errorText = "File specifying skeleton does not exist!";
            }

            if ((textBoxSequence.Text.Length == 0) || !File.Exists(textBoxSequence.Text))
            {
                error = true;
                errorText = "File specifying sequence does not exist!";
            }

            if ((textBoxVoxelSize.Text.Length == 0) || (!double.TryParse(textBoxVoxelSize.Text, out doubleTemp)))
            {
                error = true;
                errorText = "The voxel size can contain only numerical values!";
            }

            if (checkBoxMissingHelixCount.Checked && (!int.TryParse(textBoxMissingHelixCount.Text, out intTemp) || (
                int.TryParse(textBoxMissingHelixCount.Text, out intTemp) && intTemp < 0)))
            {
                error = true;
                errorText = "The missing helix count has to be a positive integer";
            }

            if ((textBoxMaxEuclideanLoopLength.Text.Length < 0) || (!int.TryParse(textBoxMaxEuclideanLoopLength.Text, out intTemp) || (
                int.TryParse(textBoxMaxEuclideanLoopLength.Text, out intTemp) && intTemp < 0)))
            {
                error = true;
                errorText = "The maximum euclidean loop length has to be a positive integer";
            }

            if (checkBoxMissingSheetCount.Checked && (!int.TryParse(textBoxMissingSheetCount.Text, out intTemp) || (
                int.TryParse(textBoxMissingSheetCount.Text, out intTemp) && intTemp < 0)))
            {
                error = true;
                errorText = "The missing sheet count has to be a positive integer";
            }

            if (!double.TryParse(textBoxMissingHelixPenalty.Text, out doubleTemp) || (double.TryParse(textBoxMissingHelixPenalty.Text, out doubleTemp) && doubleTemp < 0))
            {
                error = true;
                errorText = "The \"Helix is missing\" penalty has to be a positive number";
            }

            if (!double.TryParse(textBoxStartEndMissingHelixPenalty.Text, out doubleTemp) || (double.TryParse(textBoxStartEndMissingHelixPenalty.Text, out doubleTemp) && doubleTemp < 0))
            {
                error = true;
                errorText = "The \"First or last helix is missing\" penalty has to be a positive number";
            }

            if (!double.TryParse(textBoxEuclideanLoopPenalty.Text, out doubleTemp) || (double.TryParse(textBoxEuclideanLoopPenalty.Text, out doubleTemp) && doubleTemp < 0))
            {
                error = true;
                errorText = "The \"Euclidean loop used\" penalty has to be a positive number";
            }

            if (!double.TryParse(textBoxHelixWeightCoeff.Text, out doubleTemp) || (double.TryParse(textBoxHelixWeightCoeff.Text, out doubleTemp) && doubleTemp < 0))
            {
                error = true;
                errorText = "The Helix importance coefficient has to be a positive number";
            }

            if (!double.TryParse(textBoxLoopWeightCoeff.Text, out doubleTemp) || (double.TryParse(textBoxLoopWeightCoeff.Text, out doubleTemp) && doubleTemp < 0))
            {
                error = true;
                errorText = "The Loop importance coefficient has to be a positive number";
            }

            if (!double.TryParse(textBoxSheetWeightCoeff.Text, out doubleTemp) || (double.TryParse(textBoxSheetWeightCoeff.Text, out doubleTemp) && doubleTemp < 0))
            {
                error = true;
                errorText = "The Sheet importance coefficient has to be a positive number";
            }

            if (!double.TryParse(textBoxMissingHelixLength.Text, out doubleTemp) || (double.TryParse(textBoxMissingHelixLength.Text, out doubleTemp) && doubleTemp < 0))
            {
                error = true;
                errorText = "The average missing helix length has to be a positive number";
            }


            //textBoxEuclideanPDBRatio.Text = "10";
            //textBoxBorderMarginThreshold.Text = "2";

            if (showError && error)
            {
                MessageBox.Show(errorText, "Validation Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            return !error;
        }
        private void WriteSettingsFile(string settingsFileName)
        {
            TextWriter settings = new StreamWriter(settingsFileName);

            settings.WriteLine("SSE_FILE_NAME {0}", textBoxHelixLengths.Text);
            settings.WriteLine("VRML_HELIX_FILE_NAME {0}", textBoxHelixLocations.Text);
            settings.WriteLine("VRML_SHEET_FILE_NAME {0}", textBoxSheetLocations.Text);
            settings.WriteLine("PDB_FILE_NAME {0}", textBoxSequence.Text);
            settings.WriteLine("MRC_FILE_NAME {0}", textBoxSkeleton.Text);
            settings.WriteLine("EUCLIDEAN_DISTANCE_THRESHOLD {0}", textBoxMaxEuclideanLoopLength.Text);
            settings.WriteLine("BORDER_MARGIN_THRESHOLD {0}", textBoxBorderMarginThreshold.Text);
            settings.WriteLine("NORMALIZE_GRAPHS {0}", "1");
            settings.WriteLine("MISSING_HELIX_PENALTY {0}", textBoxMissingHelixPenalty.Text);
            settings.WriteLine("EUCLIDEAN_LOOP_PENALTY {0}", textBoxEuclideanLoopPenalty.Text);
            settings.WriteLine("START_END_MISSING_HELIX_PENALTY {0}", textBoxStartEndMissingHelixPenalty.Text);
            settings.WriteLine("HELIX_WEIGHT_COEFFICIENT {0}", textBoxHelixWeightCoeff.Text);
            settings.WriteLine("LOOP_WEIGHT_COEFFICIENT {0}", textBoxLoopWeightCoeff.Text);
            settings.WriteLine("SHEET_WEIGHT_COEFFICIENT {0}", textBoxSheetWeightCoeff.Text);
            settings.WriteLine("MISSING_HELIX_LENGTH {0}", textBoxMissingHelixLength.Text);
            settings.WriteLine("VOXEL_SIZE {0}", textBoxVoxelSize.Text);
            settings.WriteLine("EUCLIDEAN_VOXEL_TO_PDB_RATIO {0}", textBoxEuclideanPDBRatio.Text);
            settings.WriteLine("TRANSLATE_VOLUMETRIC_COORDINATES {0}", checkBoxRepositionSkeleton.Checked ? "1" : "0");
            settings.WriteLine("COST_FUNCTION {0}", radioButtonAbsoluteDifference.Checked ? "1" : radioButtonNormalizedDifference.Checked ? "2" : "3");
            settings.WriteLine("MISSING_HELIX_COUNT {0}", checkBoxMissingHelixCount.Checked ? textBoxMissingHelixCount.Text : "-1");
            settings.WriteLine("MISSING_SHEET_COUNT {0}", checkBoxMissingSheetCount.Checked ? textBoxMissingSheetCount.Text : "-1");
            settings.Close();
        }
        private void ExecuteShellCommand(string executable, string arguments)
        {
            richTextBoxOutput.Text = string.Format("{0}{1}", richTextBoxOutput.Text, "\n-------------------------\n\n\n");
            Process proc = new System.Diagnostics.Process();
            proc.EnableRaisingEvents = true;
            proc.StartInfo.FileName = executable;
            proc.StartInfo.WorkingDirectory = Application.StartupPath;
            proc.StartInfo.Arguments = arguments;
            proc.StartInfo.RedirectStandardOutput = true;
            proc.StartInfo.UseShellExecute = false;
            proc.StartInfo.CreateNoWindow = true;
            proc.OutputDataReceived += new DataReceivedEventHandler(proc_OutputDataReceived);
            proc.Exited += new EventHandler(proc_Exited);
            proc.Start();
            proc.BeginOutputReadLine();

        }
        private void SetLog(string data)
        {
            richTextBoxOutput.Text = string.Format("{0}{1}\n", richTextBoxOutput.Text, data);
            richTextBoxOutput.Refresh();
        }     
        private void RunGraphMatching()
        {
            if (ValidateForm(true))
            {
                richTextBoxOutput.Cursor = Cursors.WaitCursor;
                this.Cursor = Cursors.WaitCursor;
                tabControl.SelectedTab = tabPageOutput;
                string settingsFile = string.Format("{0}\\settings.txt", Application.StartupPath);
                WriteSettingsFile(settingsFile);
                ExecuteShellCommand(string.Format("{0}\\GraphMatch.exe", Application.StartupPath), settingsFile);                
            }
        }
        void SetArrowCursor()
        {
            richTextBoxOutput.Cursor = Cursors.Arrow;
            this.Cursor = Cursors.Arrow;
        }

        #endregion

        #region Event Handlers
        private void buttonBrowseHelixLengths_Click(object sender, EventArgs e)
        {
            SelectFile( "Select File Containing the Helix Lengths",  "SSE Files|*.sse", textBoxHelixLengths );
        }
        private void buttonBrowseHelixLocations_Click(object sender, EventArgs e)
        {
            SelectFile("Select File Containing the 3D Helix Locations", "VRML Files|*.wrl", textBoxHelixLocations);
        }
        private void buttonBrowseSheetLocations_Click(object sender, EventArgs e)
        {
            SelectFile("Select File Containing the 3D Sheet Locations", "VRML Files|*.wrl", textBoxSheetLocations);
        }
        private void buttonBrowseSkeleton_Click(object sender, EventArgs e)
        {
            SelectFile("Select File Containing the 3D Skeleton", "Cryo-EM MRC Files|*.mrc", textBoxSkeleton);
        }
        private void buttonBrowseSequence_Click(object sender, EventArgs e)
        {
            SelectFile("Select File Containing the protein sequence", "Protein Datab Bank Files|*.pdb", textBoxSequence);
        }
        private void checkBoxMissingHelixCount_CheckedChanged(object sender, EventArgs e)
        {
            textBoxMissingHelixCount.Enabled = checkBoxMissingHelixCount.Checked;
        }
        private void checkBoxMissingSheetCount_CheckedChanged(object sender, EventArgs e)
        {
            textBoxMissingSheetCount.Enabled = checkBoxMissingSheetCount.Checked;
        }
        private void buttonReset_Click(object sender, EventArgs e)
        {
            ResetForm(true);
        }
        private void buttonStart_Click(object sender, EventArgs e)
        {
            RunGraphMatching();
        }

        private void proc_OutputDataReceived(object sender, DataReceivedEventArgs e)
        {
            richTextBoxOutput.Invoke(new SetLogDelegate(SetLog), new object[] { e.Data });

        }

        private void  proc_Exited(object sender, EventArgs e)
        {
            MessageBox.Show(string.Format("MRC Files for the possible solutions created at \"{0}\"", Application.StartupPath), "Completed!", MessageBoxButtons.OK, MessageBoxIcon.Information);
            this.Invoke(new SetArrowCursorDelegate(SetArrowCursor));
        }
        #endregion

    }
}