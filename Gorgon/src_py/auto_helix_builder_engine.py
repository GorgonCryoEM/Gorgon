from math import *
from libpyGORGON import PDBAtom, Vector3DFloat, LinearSolver

class AutoHelixBuilderEngine:
    def __init__(self, patoms, score_thresh, pt_line_dist_thresh):
        self.pseudoatoms = patoms
        self.pseudoatoms.sort(None, PDBAtom.getResSeq) #sort the pseudoatoms by Residue sequence number
        self.thresh = score_thresh
        self.pt_line_dist_thresh = pt_line_dist_thresh
        self.dist_matrix = self.__get_distance_matrix() #self.dist_matrix[row][col]
        self.cell_radius = self.get_cell_radius()
    def get_helix_list(self):
        print "AutoHelixBuilder.get_helix_list"
        helix_list = []
        current_helix=[]
        possible_helix_atoms = [atom for atom in self.pseudoatoms if atom.getTempFactor() > self.thresh]
        possible_helix_atoms.sort(key=PDBAtom.getTempFactor, reverse=True) #sort by decreasing SSEHunter score
        
        while possible_helix_atoms:
            build_left = False
            build_right = False
            print "Finding helices..."
            seed_atom = possible_helix_atoms.pop(0)
            atoms_in_cell = self.get_sorted_cell_atoms(seed_atom, possible_helix_atoms)
            current_helix = [seed_atom]
            for cell_atom in atoms_in_cell:
                current_helix.append(cell_atom)
                if len(current_helix) > 2:
                    if self.is_linear(current_helix):
                        possible_helix_atoms.remove(cell_atom)
                        print "close enough to linear"
                        if len(current_helix) == 3:
                            pos = [atom.getPosition() for atom in current_helix]
                            if (pos[2]-pos[1]).length() > (pos[2]-pos[0]).length():
                                current_helix = [current_helix[2],current_helix[0],current_helix[1]]
                                (build_left, build_right) = (True, True)
                            else:
                                (build_left, build_right) = (False, True)
                            break
                    else:
                        print "not close enough to linear"
                        current_helix.remove(cell_atom)
            while build_left or build_right:
                print current_helix
                if build_left:
                    print "build_left"
                    build_left = False
                    atoms_in_cell = self.get_sorted_cell_atoms(current_helix[0], possible_helix_atoms)
                    for cell_atom in atoms_in_cell:
                        current_helix.insert(0,cell_atom)
                        if self.is_linear(current_helix):
                            possible_helix_atoms.remove(cell_atom)
                            build_left = True
                            break
                        else:
                            current_helix.pop(0)
                if build_right:
                    print "build_right"
                    build_right = False
                    atoms_in_cell = self.get_sorted_cell_atoms(current_helix[-1], possible_helix_atoms)
                    for cell_atom in atoms_in_cell:
                        current_helix.append(cell_atom)
                        if self.is_linear(current_helix):
                            possible_helix_atoms.remove(cell_atom)
                            build_right = True
                            break
                        else:
                            current_helix.pop(-1)
            
            if len(current_helix) >= 3:
                print current_helix
                helix_list.append(current_helix)
        
        for helix in helix_list:
            print helix, "\v"
        return helix_list
    def __get_distance_matrix(self):
        dist_matrix = []
        N = len(self.pseudoatoms)
        for row_num in range(N):
            row = [ (self.pseudoatoms[col_num].getPosition()-self.pseudoatoms[row_num].getPosition()).length() for col_num in range(N) ]
            dist_matrix.append(row)
        return dist_matrix
    def get_cell_radius(self):
        K = 1.5 #proportionality constant
        
        N = len(self.pseudoatoms)
        sum_distances = 0
        for i in range(N):
            nearest_neighbor = self.nearest_neighbor(i)
            distance = (nearest_neighbor.getPosition() - self.pseudoatoms[i].getPosition()).length()
            sum_distances += distance
        avg_distance = sum_distances/float(N)
        return K*avg_distance
    def get_proximity_score(self, atom1, atom2):
        distance = (atom1.getPosition() - atom2.getPosition()).length()
        optimum_distance = 3.8
        coeff = 2.0
        return exp(-coeff*(distance-optimum_distance)**2)
    def get_sorted_cell_atoms(self, central_atom, atom_list):
        atoms_in_cell = [atom for atom in atom_list if ( atom.getPosition() - central_atom.getPosition() ).length() < self.cell_radius]
        def sort_key(atom):
            proximity_weight = 0
            ssehunter_score_weight = 0
            return proximity_weight*self.get_proximity_score(atom, central_atom) + ssehunter_score_weight*atom.getTempFactor()
        atoms_in_cell.sort(key=sort_key, reverse=True)
        return atoms_in_cell
    def is_linear(self, atom_list):
        start_pt = Vector3DFloat(0,0,0)
        end_pt = Vector3DFloat(0,0,0)
        positions = [atom.getPosition() for atom in atom_list]
        LinearSolver.findBestFitLine(start_pt, end_pt, positions)
        avg_ortho_distance = sqrt( LinearSolver.sumDistSqrd(start_pt, end_pt, positions) ) / len(positions)
        return avg_ortho_distance <= self.pt_line_dist_thresh
    def nearest_neighbor(self,atom_num):
        distances = self.dist_matrix[atom_num][:] #distances to each atom
        distances.pop(atom_num) #distance of atom to itself is zero
        min_distance = min(distances)
        nearest_neighbor_index = self.dist_matrix[atom_num].index(min_distance)
        return self.pseudoatoms[nearest_neighbor_index]
