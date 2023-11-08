/** This file is part of the RHybrid simulation.
 *
 *  Copyright 2018- Aalto University
 *  Copyright 2015- Finnish Meteorological Institute
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PARTICLE_BOUNDARY_COND_HYBRID_H
#define PARTICLE_BOUNDARY_COND_HYBRID_H

#include <base_class_particle_boundary_condition.h>
#include <hybrid.h>

template<class SPECIES,class PARTICLE>
class ParticleBoundaryCondHybrid: public ParticleBoundaryCondBase {
 public:
   ParticleBoundaryCondHybrid();
   ~ParticleBoundaryCondHybrid();
   
   bool addConfigFileItems(ConfigReader& cr,const std::string& regionName);
   bool apply(pargrid::DataID particleDataID,unsigned int* N_particles,const std::vector<pargrid::CellID>& exteriorBlocks);
   bool finalize();
   bool initialize(Simulation& sim,SimulationClasses& simClasses,ConfigReader& cr,
		   const std::string& regionName,const ParticleListBase* plist);
   
 private:
   SPECIES species;
};

template<class SPECIES,class PARTICLE> inline
ParticleBoundaryCondBase* HybridBoundaryCondMaker() {return new ParticleBoundaryCondHybrid<SPECIES,PARTICLE>();}

template<class SPECIES,class PARTICLE> inline
ParticleBoundaryCondHybrid<SPECIES,PARTICLE>::ParticleBoundaryCondHybrid() { }

template<class SPECIES,class PARTICLE> inline
ParticleBoundaryCondHybrid<SPECIES,PARTICLE>::~ParticleBoundaryCondHybrid() { }

template<class SPECIES,class PARTICLE> inline
bool ParticleBoundaryCondHybrid<SPECIES,PARTICLE>::addConfigFileItems(ConfigReader& cr,const std::string& regionName) {
   return true;
}

template<class SPECIES,class PARTICLE> inline
bool ParticleBoundaryCondHybrid<SPECIES,PARTICLE>::apply(pargrid::DataID particleDataID,unsigned int* N_particles,
							 const std::vector<pargrid::CellID>& exteriorBlocks) {
   pargrid::DataWrapper<PARTICLE> wrapper = simClasses->pargrid.getUserDataDynamic<PARTICLE>(particleDataID);

   // filter corrupted particles after a restart
   if(sim->restarted == true && Hybrid::filterParticlesAfterRestartDone == false) {
       simClasses->logger << "(RHYBRID) Removing corrupted macroparticles after a restart (" << species.name << ")" << std::endl;
       Real N_macroParticles = 0.0;
       Real N_badMacroParticles = 0.0;
       Real cp1 = 0.0;
       Real cp2 = 0.0;
       Real cp3 = 0.0;
       Real cp4 = 0.0;
       Real cp5 = 0.0;
       Real cp6 = 0.0;
       Real cp7 = 0.0;
       Real cp8 = 0.0;
       Real cp9 = 0.0;
       Real cp10 = 0.0;
       for(pargrid::CellID b=0; b<simClasses->pargrid.getNumberOfLocalCells(); ++b) {
	   // get block sizes
	   Real bs[3];
	   getBlockSize(*simClasses,*sim,b,bs);
	   PARTICLE* particles = wrapper.data()[b];
	   int current = 0;
	   int end = N_particles[b]-1;
	   while (current <= end) {
	       N_macroParticles++;
	       const Real x = particles[current].state[particle::X];
	       const Real y = particles[current].state[particle::Y];
	       const Real z = particles[current].state[particle::Z];
	       const Real vx = particles[current].state[particle::VX];
	       const Real vy = particles[current].state[particle::VY];
	       const Real vz = particles[current].state[particle::VZ];
	       const Real w = particles[current].state[particle::WEIGHT];
	       // check coordinates, velocities and weight: remove if the particle is corrupted
	       if(x < 0 || y < 0 || z < 0 || x > bs[0] || y > bs[1] || z > bs[2] ||
		  fabs(vx) > Hybrid::maxVi || fabs(vy) > Hybrid::maxVi || fabs(vz) > Hybrid::maxVi ||
		  w != Hybrid::allPops[species.popid-1].w ||
		  std::isnan(x) == true || std::isnan(y) == true || std::isnan(z) == true ||
		  std::isnan(vx) == true || std::isnan(vy) == true || std::isnan(vz) == true ||
		  std::isnan(w) == true) {/*
		   particles[current] = particles[end];
		   --end;
		   N_badMacroParticles++;
		   continue;*/
	       }
		if(x < 0 || std::isnan(x) == true) {
		  //simClasses->logger << "Corrpt1 bc x < 0! " << std::endl;
		  //simClasses->logger << "x = " << x << std::endl;
                  particles[current] = particles[end];
                  --end;
                  N_badMacroParticles++;
                  cp1++;
		  continue;}
		if(y < 0 || std::isnan(y) == true) {
                  //simClasses->logger << "Corrpt2 bc y < 0! " << std::endl;
                  //simClasses->logger << "y = " << y << std::endl;
                  particles[current] = particles[end];
                  --end;
                  N_badMacroParticles++;
                  cp2++;
                  continue;}
		if(z < 0 || std::isnan(z) == true) {
                  //simClasses->logger << "Corrpt3 bc z < 0! " << std::endl;
                  //simClasses->logger << "z = " << z << std::endl;
                  particles[current] = particles[end];
                  --end;
                  N_badMacroParticles++;
                  cp3++;
                  continue;}
                if(x > bs[0]) {
                  //simClasses->logger << "Corrpt4 bc x > bs[0]! " << std::endl;
                  //simClasses->logger << "x = " << x << ", bs[0] = " << bs[0] << std::endl;
                  particles[current] = particles[end];
                  --end;
                  N_badMacroParticles++;
                  cp4++;
                  continue;}
                if(y > bs[1]) {
                  //simClasses->logger << "Corrpt5 bc y > bs[1]! " << std::endl;
                  //simClasses->logger << "y = " << y << std::endl;
                  particles[current] = particles[end];
                  --end;
                  N_badMacroParticles++;
                  cp5++;
                  continue;}
                if(z > bs[2]) {
                  //simClasses->logger << "Corrpt6 bc z > bs[2]! " << std::endl;
                  //simClasses->logger << "z = " << z << std::endl;
                  particles[current] = particles[end];
                  --end;
                  N_badMacroParticles++;
                  cp6++;
                  continue;}
                if(fabs(vx) > Hybrid::maxVi || std::isnan(vx) == true) {
                  //simClasses->logger << "Corrpt7 bc vx > max_v! " << std::endl;
                  //simClasses->logger << "vx = " << fabs(vx) << std::endl;
                  particles[current] = particles[end];
                  --end;
                  N_badMacroParticles++;
                  cp7++;
                  continue;}
                if(fabs(vy) > Hybrid::maxVi || std::isnan(vy) == true) {
                  //simClasses->logger << "Corrpt8 bc vy > max_v! " << std::endl;
                  //simClasses->logger << "vy = " << fabs(vy) << std::endl;
                  particles[current] = particles[end];
                  --end;
                  N_badMacroParticles++;
                  cp8++;
                  continue;}
                if(fabs(vz) > Hybrid::maxVi || std::isnan(vz) == true) {
                  //simClasses->logger << "Corrpt9 bc vz > max_v! " << std::endl;
                  //simClasses->logger << "vz = " << fabs(vz) << std::endl;
                  particles[current] = particles[end];
                  --end;
                  N_badMacroParticles++;
                  cp9++;
                  continue;}
                if(w != Hybrid::allPops[species.popid-1].w || std::isnan(w) == true) {
                  //simClasses->logger << "Corrpt10 bc w!= AllPop.w " << std::endl;
                  //simClasses->logger << "w = " << w << std::endl;
                  //simClasses->logger << "AllPop.w = " << Hybrid::allPops[species.popid-1].w << std::endl;
                  particles[current] = particles[end];
                  --end;
                  N_badMacroParticles++;
                  cp10++;
                  continue;}
	       ++current;
	   }
	   wrapper.resize(b,current);
	   N_particles[b] = current;
       }
       Real N_macroParticlesGlobal = 0.0;
       Real N_badMacroParticlesGlobal = 0.0;
       MPI_Reduce(&N_macroParticles,&N_macroParticlesGlobal,1,MPI_Type<Real>(),MPI_SUM,sim->MASTER_RANK,sim->comm);
       MPI_Reduce(&N_badMacroParticles,&N_badMacroParticlesGlobal,1,MPI_Type<Real>(),MPI_SUM,sim->MASTER_RANK,sim->comm);
       simClasses->logger << "Species: " << species.name << " cp1 = "<< cp1 << std::endl;
       simClasses->logger << "Species: " << species.name << " cp2 = "<< cp2 << std::endl;
       simClasses->logger << "Species: " << species.name << " cp3 = "<< cp3 << std::endl;
       simClasses->logger << "Species: " << species.name << " cp4 = "<< cp4 << std::endl;
       simClasses->logger << "Species: " << species.name << " cp5 = "<< cp5 << std::endl;
       simClasses->logger << "Species: " << species.name << " cp6 = "<< cp6 << std::endl;
       simClasses->logger << "Species: " << species.name << " cp7 = "<< cp7 << std::endl;
       simClasses->logger << "Species: " << species.name << " cp8 = "<< cp8 << std::endl;
       simClasses->logger << "Species: " << species.name << " cp9 = "<< cp9 << std::endl;
       simClasses->logger << "Species: " << species.name << " cp10 = "<< cp10 << std::endl;
       simClasses->logger << "Species: " << species.name << " Number of particles = "<< N_macroParticles << std::endl; 
       if(sim->mpiRank == sim->MASTER_RANK) {
	   simClasses->logger
	       << "Number of corrupted macroparticles removed: "
	       << real2str(N_badMacroParticlesGlobal,15) << " (" << species.name << ")" << std::endl;
	   simClasses->logger
	       << "Number of macroparticles checked: "
	       << real2str(N_macroParticlesGlobal,15) << " (" << species.name << ")" << std::endl << write;
       }
       return true;
   }
   
   // Remove particles on exterior cells:
   Real t_propag = 0.0;
   for (pargrid::CellID b=0; b<exteriorBlocks.size(); ++b) {
      // Measure computation time if we are testing for repartitioning:
      if (sim->countPropagTime == true) t_propag = MPI_Wtime();
      
      const pargrid::CellID blockLID = exteriorBlocks[b];
      PARTICLE* particles = wrapper.data()[blockLID];
      for(size_t p=0; p<N_particles[blockLID]; ++p) {
	 // escape counter
	 Hybrid::particleCounterEscape[species.popid-1] += particles[p].state[particle::WEIGHT];
	 Hybrid::particleCounterEscapeKineticEnergy[species.popid-1] += particles[p].state[particle::WEIGHT]*( sqr(particles[p].state[particle::VX]) + sqr(particles[p].state[particle::VY]) + sqr(particles[p].state[particle::VZ]) );
      }
      N_particles[blockLID] = 0;
      wrapper.resize(blockLID,0);
      
      // Store block injection time:
      if (sim->countPropagTime == true) {
	 t_propag = std::max(0.0,MPI_Wtime() - t_propag);
	 simClasses->pargrid.getCellWeights()[blockLID] += t_propag;
      }
   }

   // Inner boundary
   bool* innerFlagParticle = simClasses->pargrid.getUserDataStatic<bool>(Hybrid::dataInnerFlagParticleID);
   const double* crd = getBlockCoordinateArray(*sim,*simClasses);
   for (pargrid::CellID b=0; b<simClasses->pargrid.getNumberOfLocalCells(); ++b) {
      if (innerFlagParticle[b] == false) { continue; }
      
      // Measure computation time if we are testing for repartitioning:
      if (sim->countPropagTime == true) t_propag = MPI_Wtime();
      PARTICLE* particles = wrapper.data()[b];
#ifdef USE_CONIC_INNER_BOUNDARY
      // remove all particles within the conic inner boundary cells
      int current = 0;
      int end = N_particles[b]-1;
      while (current <= end) {
         Hybrid::particleCounterImpact[this->species.popid-1] += particles[current].state[particle::WEIGHT];
	 Hybrid::particleCounterImpactKineticEnergy[this->species.popid-1] += particles[current].state[particle::WEIGHT]*( sqr(particles[current].state[particle::VX]) + sqr(particles[current].state[particle::VY]) + sqr(particles[current].state[particle::VZ]) );
         particles[current] = particles[end];
         --end;
      }
#else
      const size_t b3 = 3*b;
      const Real xBlock = crd[b3+0];
      const Real yBlock = crd[b3+1];
      const Real zBlock = crd[b3+2];
      int current = 0;
      int end = N_particles[b]-1;
      while (current <= end) {
	 const Real r2 =  sqr(xBlock + particles[current].state[particle::X]) +
	   sqr(yBlock + particles[current].state[particle::Y]) +
	   sqr(zBlock + particles[current].state[particle::Z]);
	 //if (r2 < Hybrid::R2_particleObstacle) {
         if (r2 < this->species.R2_obstacle) {
	    // impact counter
	    Hybrid::particleCounterImpact[this->species.popid-1] += particles[current].state[particle::WEIGHT];
	    Hybrid::particleCounterImpactKineticEnergy[this->species.popid-1] += particles[current].state[particle::WEIGHT]*( sqr(particles[current].state[particle::VX]) + sqr(particles[current].state[particle::VY]) + sqr(particles[current].state[particle::VZ]) );
	    particles[current] = particles[end];
	    --end;
	    continue;
	 }
	 ++current;
      }
#endif
      wrapper.resize(b,current);
      N_particles[b] = current;
      // Store block injection time:
      if (sim->countPropagTime == true) {
	 t_propag = std::max(0.0,MPI_Wtime() - t_propag);
	 simClasses->pargrid.getCellWeights()[b] += t_propag;
      }
   }

   return true;
}

template<class SPECIES,class PARTICLE> inline
bool ParticleBoundaryCondHybrid<SPECIES,PARTICLE>::finalize() {
   return true;
}

template<class SPECIES,class PARTICLE> inline
bool ParticleBoundaryCondHybrid<SPECIES,PARTICLE>::initialize(Simulation& sim,SimulationClasses& simClasses,ConfigReader& cr,
							      const std::string& regionName,const ParticleListBase* plist) {
   bool success = ParticleBoundaryCondBase::initialize(sim,simClasses,cr,regionName,plist);
   species = *reinterpret_cast<const SPECIES*>(plist->getSpecies());
   return success;
}

#endif
