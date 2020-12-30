/* vim: set sw=4 sts=4 et tw=120 foldmethod=syntax : */

/*
 * Copyright (c) 2020 Danny van Dyk
 *
 * This file is part of the EOS project. EOS is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * EOS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef EOS_GUARD_EOS_FORM_FACTORS_PARAMETRIC_EGJVD2020_IMPL_HH
#define EOS_GUARD_EOS_FORM_FACTORS_PARAMETRIC_EGJVD2020_IMPL_HH 1

#include <eos/form-factors/parametric-egjvd2020.hh>
#include <eos/form-factors/mesonic.hh>
#include <eos/utils/complex.hh>
#include <eos/utils/parameters.hh>
#include <eos/utils/options.hh>
#include <eos/utils/qualified-name.hh>
#include <eos/utils/stringify.hh>

namespace eos
{
    struct VacuumToPiPi {
        typedef VacuumToPP Transition;
        static constexpr const char * label = "0->pipi";
        static constexpr const double m_1 = 0.137; //GeV
        static constexpr const double m_2 = 0.137; //GeV
        static constexpr const double t_p = (m_1 + m_2) * (m_1 + m_2);
        static constexpr const double t_m = (m_1 - m_2) * (m_1 - m_2);
        static constexpr const double Q2  = 2.0;
        static constexpr const bool has_scalar_form_factor = false;
        static constexpr const auto asymptotic_case_switch = 1; // makeshift implementation of switching the different ayptotic behaviours
    };

    struct PiToPi {
        typedef PToP Transition;
        static constexpr const char * label = "0->pipi"; // TODO
        static constexpr const double m_1 = 0.137; //GeV
        static constexpr const double m_2 = 0.137; //GeV
        static constexpr const double t_p = (m_1 + m_2) * (m_1 + m_2); // = 1.87e-8
        static constexpr const double t_m = (m_1 - m_2) * (m_1 - m_2);
        static constexpr const double Q2  = 2.0;
        static constexpr const bool has_scalar_form_factor = false;
        static constexpr const auto asymptotic_case_switch = 1; // makeshift implementation of switching the different ayptotic behaviours
    };

    template <typename Process_, typename Transition_, bool has_scalar_form_factor_> class EGJvD2020UnitarityBoundsBase;

    template <typename Process_> class EGJvD2020UnitarityBoundsBase<Process_, VacuumToPP, false> :
        public virtual ParameterUser
    {
        private:
            // parameters for form factors f_+ and f_T
            std::array<UsedParameter, 10u> _a_fp, _a_ft;

            std::string _par_name(const std::string & ff, const std::string & index) const
            {
                return stringify(Process_::label) + "::" + "a_" + ff + "^" + index + "@EGJvD2020";
            }
        public:
            EGJvD2020UnitarityBoundsBase(const Parameters & p, const Options & o) :
                _a_fp{{
                    UsedParameter(p[_par_name("+", "0")], *this),
                    UsedParameter(p[_par_name("+", "1")], *this),
                    UsedParameter(p[_par_name("+", "2")], *this),
                    UsedParameter(p[_par_name("+", "3")], *this),
                    UsedParameter(p[_par_name("+", "4")], *this),

                    UsedParameter(p[_par_name("+", "5")], *this),
                    UsedParameter(p[_par_name("+", "6")], *this),
                    UsedParameter(p[_par_name("+", "7")], *this),
                    UsedParameter(p[_par_name("+", "8")], *this),
                    UsedParameter(p[_par_name("+", "9")], *this)
                }},
                _a_ft{{
                    UsedParameter(p[_par_name("T", "0")], *this),
                    UsedParameter(p[_par_name("T", "1")], *this),
                    UsedParameter(p[_par_name("T", "2")], *this),
                    UsedParameter(p[_par_name("T", "3")], *this),
                    UsedParameter(p[_par_name("T", "4")], *this),

                    UsedParameter(p[_par_name("T", "5")], *this),
                    UsedParameter(p[_par_name("T", "6")], *this),
                    UsedParameter(p[_par_name("T", "7")], *this),
                    UsedParameter(p[_par_name("T", "8")], *this),
                    UsedParameter(p[_par_name("T", "9")], *this)
                }}
            {
            }

            ~EGJvD2020UnitarityBoundsBase() = default;

            double bound_1m_prior() const
            {
                const double value = bound_1m();

                if (value < 0.0)
                {
                    throw InternalError("Contribution to 1^- unitarity bound must be positive; found to be negative!");
                }
                else if ((0.0 <= value) && (value < 1.0))
                {
                    return 0.0;
                }
                else
                {
                    // add an r-fit like penalty
                    static const double sigma = 0.05; // TODO preliminary assuming 5% uncertainty
                    return -pow((value - 1.0) / sigma, 2) / 2.0;
                }
            }

            double bound_1m() const
            {
                double sum = 0.0;
                for (auto & a : _a_fp)
                {
                    sum += a * a;
                }

                return sum;
            }
    };

    template <typename Process_> class EGJvD2020UnitarityBounds:
        public EGJvD2020UnitarityBoundsBase<Process_, typename Process_::Transition, Process_::has_scalar_form_factor>
    {
        public:
            EGJvD2020UnitarityBounds(const Parameters & p, const Options & o) :
                EGJvD2020UnitarityBoundsBase<Process_, typename Process_::Transition, Process_::has_scalar_form_factor>(p, o)
            {
            }

            ~EGJvD2020UnitarityBounds() = default;
    };

    template <typename Process_, typename Transition_, bool has_scalar_form_factor_> class EGJvD2020FormFactorBase;

    template <typename Process_> class EGJvD2020FormFactorBase<Process_, VacuumToPP, false> :
        public FormFactors<VacuumToPP>
    {
        private:
            // parameters for form factors f_+ and f_T
            std::array<UsedParameter, 10u> _a_fp, _a_ft;

            // parameter for zero point of z
            UsedParameter _t_0;

            std::string _par_name(const std::string & ff, const std::string & index) const
            {
                return stringify(Process_::label) + "::" + "a_" + ff + "^" + index + "@EGJvD2020";
            }

            complex<double> _z(const double & q2, const double & t_0) const
            {   
                const double t_p = Process_::t_p;
                if (q2 > t_p) {
                    // assumes that Re(q2) > t_p and Im(q2) < 0 such that Im(z) > 0.
                    const double re = (q2 + t_0 - 2.0 * t_p) / (q2 - t_0);
                    const double im = 2.0 * sqrt((q2 - t_p) * (t_p - t_0)) / (q2 - t_0);
                    return complex<double>{ re, im };
                } else {
                    const double a = sqrt(t_p - t_0);
                    const double re = (sqrt(t_p - q2) - a) / (sqrt(t_p - q2) + a);
                    const double im = 0.0;
                    return complex<double>{ re, im };
                }
            }

            complex<double> _z(const double & q2) const
            {
                return _z(q2, this->_t_0);
            }

        public:
            EGJvD2020FormFactorBase(const Parameters & p, const Options &) :
                _a_fp{{
                    UsedParameter(p[_par_name("+", "0")], *this),
                    UsedParameter(p[_par_name("+", "1")], *this),
                    UsedParameter(p[_par_name("+", "2")], *this),
                    UsedParameter(p[_par_name("+", "3")], *this),
                    UsedParameter(p[_par_name("+", "4")], *this),

                    UsedParameter(p[_par_name("+", "5")], *this),
                    UsedParameter(p[_par_name("+", "6")], *this),
                    UsedParameter(p[_par_name("+", "7")], *this),
                    UsedParameter(p[_par_name("+", "8")], *this),
                    UsedParameter(p[_par_name("+", "9")], *this)
                }},
                _a_ft{{
                    UsedParameter(p[_par_name("T", "0")], *this),
                    UsedParameter(p[_par_name("T", "1")], *this),
                    UsedParameter(p[_par_name("T", "2")], *this),
                    UsedParameter(p[_par_name("T", "3")], *this),
                    UsedParameter(p[_par_name("T", "4")], *this),

                    UsedParameter(p[_par_name("T", "5")], *this),
                    UsedParameter(p[_par_name("T", "6")], *this),
                    UsedParameter(p[_par_name("T", "7")], *this),
                    UsedParameter(p[_par_name("T", "8")], *this),
                    UsedParameter(p[_par_name("T", "9")], *this)
                }},
                _t_0(p[stringify(Process_::label) + "::t_0@EGJvD2020"], *this)
            {
            }

            /* f_+ */

            complex<double> phi_p(const complex<double> & z) const
            {
                // TODO (->EE): implement phi_+
                // TODO implement proper pi and chi(Q2)
                const double t_p = Process_::t_p;           // = 1.87e-8
                const double t_0 = this->_t_0;              // = -2.0
                const double tfactor = 1.0 - t_0 / t_p;     // = 1.0 for t_0 = 0
                const double chi = 0.00405;                 //GeV^-2 as a makeshift value
                const double Q2 = Process_::Q2;             // = 2.0

                const double part0 = 1.0 / sqrt(12.0 * M_PI * t_p * chi);
                const complex<double> part1 = /*(1.0 + z) * (1.0 + z) * sqrt(1.0 - z)*/ pow(tfactor, 1.25);
                const complex<double> part2 = pow(sqrt(tfactor) * (1.0 + z) + (1.0 - z), -0.5);
                const complex<double> part3 = pow(sqrt(1.0 + Q2 / t_p) * (1.0 - z) + sqrt(tfactor) * (1.0 + z), -3.0);

                return part0 * part1 * part2 * part3;
            }

            complex<double> blaschke_p(const double & q2) const
            {
                return 1.0;
            }

            complex<double> series_p(const complex<double> & z) const
            {
                complex<double> result = 0.0;

                //TODO: properly implement the options for the different polynomial choices

               switch (Process_::asymptotic_case_switch) //intermediate solution: DONT FORGET TO APPLY THIS OPTION BELOW TOO
                {
                    case 0:
                        result += _a_fp[0]() * (1.0 * pow(z, 0));
                        result += _a_fp[1]() * (-3.0 / 7.0 * pow(z, 0) + 1.0 * pow(z, 1));
                        result += _a_fp[2]() * (5.0 / 9.0 * pow(z, 0) + -2.0 / 3.0 * pow(z, 1) + 1.0 * pow(z, 2));
                        result += _a_fp[3]() * (-3.0 / 11.0 * pow(z, 0) + 73.0 / 99.0 * pow(z, 1) + -9.0 / 11.0 * pow(z, 2) + 1.0 * pow(z, 3));
                        result += _a_fp[4]() * (5.0 / 13.0 * pow(z, 0) + -84.0 / 143.0 * pow(z, 1) + 146.0 / 143.0 * pow(z, 2) + -12.0 / 13.0 * pow(z, 3) + 1.0 * pow(z, 4));
                        result += _a_fp[5]() * (-1.0 / 5.0 * pow(z, 0) + 37.0 / 65.0 * pow(z, 1) + -566.0 / 715.0 * pow(z, 2) + 74.0 / 65.0 * pow(z, 3) + -1.0 / 1.0 * pow(z, 4) + 1.0 * pow(z, 5));
                        result += _a_fp[6]() * (5.0 / 17.0 * pow(z, 0) + -42.0 / 85.0 * pow(z, 1) + 999.0 / 1105.0 * pow(z, 2) + -1132.0 / 1105.0 * pow(z, 3) + 111.0 / 85.0 * pow(z, 4) + -18.0 / 17.0 * pow(z, 5) + 1.0 * pow(z, 6));
                        result += _a_fp[7]() * (-3.0 / 19.0 * pow(z, 0) + 149.0 / 323.0 * pow(z, 1) + -1131.0 / 1615.0 * pow(z, 2) + 22377.0 / 20995.0 * pow(z, 3) + -377.0 / 323.0 * pow(z, 4) + 447.0 / 323.0 * pow(z, 5) + -21.0 / 19.0 * pow(z, 6) + 1.0 * pow(z, 7));
                        result += _a_fp[8]() * (5.0 / 21.0 * pow(z, 0) + -8.0 / 19.0 * pow(z, 1) + 1788.0 / 2261.0 * pow(z, 2) + -33176.0 / 33915.0 * pow(z, 3) + 14918.0 / 11305.0 * pow(z, 4) + -3016.0 / 2261.0 * pow(z, 5) + 596.0 / 399.0 * pow(z, 6) + -8.0 / 7.0 * pow(z, 7) + 1.0 * pow(z, 8));
                        result += _a_fp[9]() * (-3.0 / 23.0 * pow(z, 0) + 187.0 / 483.0 * pow(z, 1) + -1884.0 / 3059.0 * pow(z, 2) + 50172.0 / 52003.0 * pow(z, 3) + -179462.0 / 156009.0 * pow(z, 4) + 75258.0 / 52003.0 * pow(z, 5) + -628.0 / 437.0 * pow(z, 6) + 748.0 / 483.0 * pow(z, 7) + -27.0 / 23.0 * pow(z, 8) + 1.0 * pow(z, 9));

                        break;

                    case 1:
                        result += _a_fp[0]() * (1.0 * pow(z, 0));
                        result += _a_fp[1]() * (1.0 / 11.0 * pow(z, 0) + 1.0 * pow(z, 1));
                        result += _a_fp[2]() * (9.0 / 13.0 * pow(z, 0) + 2.0 / 13.0 * pow(z, 1) + 1.0 * pow(z, 2));
                        result += _a_fp[3]() * (1.0 / 15.0 * pow(z, 0) + 137.0 / 195.0 * pow(z, 1) + 1.0 / 5.0 * pow(z, 2) + 1.0 * pow(z, 3));
                        result += _a_fp[4]() * (9.0 / 17.0 * pow(z, 0) + 44.0 / 255.0 * pow(z, 1) + 274.0 / 255.0 * pow(z, 2) + 4.0 / 17.0 * pow(z, 3) + 1.0 * pow(z, 4));
                        result += _a_fp[5]() * (1.0 / 19.0 * pow(z, 0) + 175.0 / 323.0 * pow(z, 1) + 74.0 / 323.0 * pow(z, 2) + 350.0 / 323.0 * pow(z, 3) + 5.0 / 19.0 * pow(z, 4) + 1.0 * pow(z, 5));
                        result += _a_fp[6]() * (3.0 / 7.0 * pow(z, 0) + 22.0 / 133.0 * pow(z, 1) + 325.0 / 323.0 * pow(z, 2) + 740.0 / 2261.0 * pow(z, 3) + 25.0 / 19.0 * pow(z, 4) + 2.0 / 7.0 * pow(z, 5) + 1.0 * pow(z, 6));
                        result += _a_fp[7]() * (1.0 / 23.0 * pow(z, 0) + 71.0 / 161.0 * pow(z, 1) + 681.0 / 3059.0 * pow(z, 2) + 53065.0 / 52003.0 * pow(z, 3) + 1135.0 / 3059.0 * pow(z, 4) + 213.0 / 161.0 * pow(z, 5) + 7.0 / 23.0 * pow(z, 6) + 1.0 * pow(z, 7));
                        result += _a_fp[8]() * (9.0 / 25.0 * pow(z, 0) + 88.0 / 575.0 * pow(z, 1) + 3692.0 / 4025.0 * pow(z, 2) + 5448.0 / 15295.0 * pow(z, 3) + 21226.0 / 15295.0 * pow(z, 4) + 1816.0 / 4025.0 * pow(z, 5) + 852.0 / 575.0 * pow(z, 6) + 8.0 / 25.0 * pow(z, 7) + 1.0 * pow(z, 8));
                        result += _a_fp[9]() * (1.0 / 27.0 * pow(z, 0) + 251.0 / 675.0 * pow(z, 1) + 1076.0 / 5175.0 * pow(z, 2) + 580.0 / 621.0 * pow(z, 3) + 24046.0 / 58995.0 * pow(z, 4) + 290.0 / 207.0 * pow(z, 5) + 7532.0 / 15525.0 * pow(z, 6) + 1004.0 / 675.0 * pow(z, 7) + 1.0 / 3.0 * pow(z, 8) + 1.0 * pow(z, 9));

                        break;
                }

                return result;
            }

            virtual complex<double> f_p(const double & q2) const
            {
                const auto z = _z(q2);
                //return this->phi_p(z);
                const auto phi      = this->phi_p(z);
                const auto blaschke = this->blaschke_p(q2);
                const auto series   = this->series_p(z);
                complex<double> correction_to_asymptotics;
                switch (Process_::asymptotic_case_switch)
                {
                    case 0: // The asymptotics used by Buck and Lebed
                        correction_to_asymptotics = 1.0;
                        break;
                    case 1: // correct asyptotic behaviour
                        correction_to_asymptotics = pow(1.0 - z, 5.0 / 2.0 - 1.0 / 2.0);
                        break;
                    default:
                        correction_to_asymptotics = 1.0;
                        break;
                }
                const auto K        = (5 * M_PI) / 64;

                return 1.0 / (phi * blaschke) * correction_to_asymptotics * sqrt(K) * series; //todo combine asymptotics and phi into one
                // return 1.0 / (phi * blaschke) /* asymptotics*/ * sqrt(K) * series; //todo combine asymptotics and phi into one
            }

            /* f_T */
            virtual complex<double> f_t(const double & q2) const
            {
                return 1.0;
            }

            /* f_0 */
            virtual complex<double> f_0(const double & /*q2*/) const
            {
                return 0.0; // vanishes exactly
            }
    };
    //TODO: Check this
    template <typename Process_> class EGJvD2020FormFactorBase<Process_, PToP, false> :
        public FormFactors<PToP>
    {
        private:
            // parameters for form factors f_+ and f_T
            std::array<UsedParameter, 10u> _a_fp, _a_ft;

            // parameter for zero point of z
            UsedParameter _t_0;

            std::string _par_name(const std::string & ff, const std::string & index) const
            {
                return stringify(Process_::label) + "::" + "a_" + ff + "^" + index + "@EGJvD2020";
            }

            double _z(const double & q2, const double & t_0) const
            {
                const double t_p = Process_::t_p;
                const double a = sqrt(t_p - t_0);
                const double z = (sqrt(t_p - q2) - a) / (sqrt(t_p - q2) + a);

                return z;
            }

            double _z(const double & q2) const
            {
                return _z(q2, this->_t_0);
            }

        public:
            EGJvD2020FormFactorBase(const Parameters & p, const Options &) :
                _a_fp{{
                    UsedParameter(p[_par_name("+", "0")], *this),
                    UsedParameter(p[_par_name("+", "1")], *this),
                    UsedParameter(p[_par_name("+", "2")], *this),
                    UsedParameter(p[_par_name("+", "3")], *this),
                    UsedParameter(p[_par_name("+", "4")], *this),

                    UsedParameter(p[_par_name("+", "5")], *this),
                    UsedParameter(p[_par_name("+", "6")], *this),
                    UsedParameter(p[_par_name("+", "7")], *this),
                    UsedParameter(p[_par_name("+", "8")], *this),
                    UsedParameter(p[_par_name("+", "9")], *this)
                }},
                _a_ft{{
                    UsedParameter(p[_par_name("T", "0")], *this),
                    UsedParameter(p[_par_name("T", "1")], *this),
                    UsedParameter(p[_par_name("T", "2")], *this),
                    UsedParameter(p[_par_name("T", "3")], *this),
                    UsedParameter(p[_par_name("T", "4")], *this),

                    UsedParameter(p[_par_name("T", "5")], *this),
                    UsedParameter(p[_par_name("T", "6")], *this),
                    UsedParameter(p[_par_name("T", "7")], *this),
                    UsedParameter(p[_par_name("T", "8")], *this),
                    UsedParameter(p[_par_name("T", "9")], *this)
                }},
                _t_0(p[stringify(Process_::label) + "::t_0@EGJvD2020"], *this)
            {
            }

            /* f_+ */

            double phi_p(const double & z) const
            {
                const double t_p = Process_::t_p;
                const double t_0 = this->_t_0;
                const double tfactor = 1.0 - t_0 / t_p;
                const double chi = 0.00405; //GeV^-2 as a makeshift value
                const double Q2 = Process_::Q2;

                const double part0 = 1.0 / sqrt(12.0 * M_PI * t_p * chi); // = 
                const double part1 = /*(1.0 + z) * (1.0 + z) * sqrt(1.0 - z) */ pow(tfactor, 1.25);
                const double part2 = pow(sqrt(tfactor) * (1.0 + z) + (1.0 - z), -0.5);
                const double part3 = pow(sqrt(1.0 + Q2 / t_p) * (1.0 - z) + sqrt(tfactor) * (1.0 + z), -3.0);

                return part0 * part1 * part2 * part3;
            }

            double blaschke_p(const double & q2) const
            {
                return 1.0;
            }

            double series_p(const double & z) const
            {
                double result = 0.0;

                switch (Process_::asymptotic_case_switch) //intermediate solution: DONT FORGET TO APPLY THIS OPTION ABOVE TOO
                {
                    case 0:
                        result += _a_fp[0]() * (1.0 * pow(z, 0));
                        result += _a_fp[1]() * (-3.0 / 7.0 * pow(z, 0) + 1.0 * pow(z, 1));
                        result += _a_fp[2]() * (5.0 / 9.0 * pow(z, 0) + -2.0 / 3.0 * pow(z, 1) + 1.0 * pow(z, 2));
                        result += _a_fp[3]() * (-3.0 / 11.0 * pow(z, 0) + 73.0 / 99.0 * pow(z, 1) + -9.0 / 11.0 * pow(z, 2) + 1.0 * pow(z, 3));
                        result += _a_fp[4]() * (5.0 / 13.0 * pow(z, 0) + -84.0 / 143.0 * pow(z, 1) + 146.0 / 143.0 * pow(z, 2) + -12.0 / 13.0 * pow(z, 3) + 1.0 * pow(z, 4));
                        result += _a_fp[5]() * (-1.0 / 5.0 * pow(z, 0) + 37.0 / 65.0 * pow(z, 1) + -566.0 / 715.0 * pow(z, 2) + 74.0 / 65.0 * pow(z, 3) + -1.0 / 1.0 * pow(z, 4) + 1.0 * pow(z, 5));
                        result += _a_fp[6]() * (5.0 / 17.0 * pow(z, 0) + -42.0 / 85.0 * pow(z, 1) + 999.0 / 1105.0 * pow(z, 2) + -1132.0 / 1105.0 * pow(z, 3) + 111.0 / 85.0 * pow(z, 4) + -18.0 / 17.0 * pow(z, 5) + 1.0 * pow(z, 6));
                        result += _a_fp[7]() * (-3.0 / 19.0 * pow(z, 0) + 149.0 / 323.0 * pow(z, 1) + -1131.0 / 1615.0 * pow(z, 2) + 22377.0 / 20995.0 * pow(z, 3) + -377.0 / 323.0 * pow(z, 4) + 447.0 / 323.0 * pow(z, 5) + -21.0 / 19.0 * pow(z, 6) + 1.0 * pow(z, 7));
                        result += _a_fp[8]() * (5.0 / 21.0 * pow(z, 0) + -8.0 / 19.0 * pow(z, 1) + 1788.0 / 2261.0 * pow(z, 2) + -33176.0 / 33915.0 * pow(z, 3) + 14918.0 / 11305.0 * pow(z, 4) + -3016.0 / 2261.0 * pow(z, 5) + 596.0 / 399.0 * pow(z, 6) + -8.0 / 7.0 * pow(z, 7) + 1.0 * pow(z, 8));
                        result += _a_fp[9]() * (-3.0 / 23.0 * pow(z, 0) + 187.0 / 483.0 * pow(z, 1) + -1884.0 / 3059.0 * pow(z, 2) + 50172.0 / 52003.0 * pow(z, 3) + -179462.0 / 156009.0 * pow(z, 4) + 75258.0 / 52003.0 * pow(z, 5) + -628.0 / 437.0 * pow(z, 6) + 748.0 / 483.0 * pow(z, 7) + -27.0 / 23.0 * pow(z, 8) + 1.0 * pow(z, 9));

                        break;

                    case 1:
                        result += _a_fp[0]() * (1.0 * pow(z, 0));
                        result += _a_fp[1]() * (1.0 / 11.0 * pow(z, 0) + 1.0 * pow(z, 1));
                        result += _a_fp[2]() * (9.0 / 13.0 * pow(z, 0) + 2.0 / 13.0 * pow(z, 1) + 1.0 * pow(z, 2));
                        result += _a_fp[3]() * (1.0 / 15.0 * pow(z, 0) + 137.0 / 195.0 * pow(z, 1) + 1.0 / 5.0 * pow(z, 2) + 1.0 * pow(z, 3));
                        result += _a_fp[4]() * (9.0 / 17.0 * pow(z, 0) + 44.0 / 255.0 * pow(z, 1) + 274.0 / 255.0 * pow(z, 2) + 4.0 / 17.0 * pow(z, 3) + 1.0 * pow(z, 4));
                        result += _a_fp[5]() * (1.0 / 19.0 * pow(z, 0) + 175.0 / 323.0 * pow(z, 1) + 74.0 / 323.0 * pow(z, 2) + 350.0 / 323.0 * pow(z, 3) + 5.0 / 19.0 * pow(z, 4) + 1.0 * pow(z, 5));
                        result += _a_fp[6]() * (3.0 / 7.0 * pow(z, 0) + 22.0 / 133.0 * pow(z, 1) + 325.0 / 323.0 * pow(z, 2) + 740.0 / 2261.0 * pow(z, 3) + 25.0 / 19.0 * pow(z, 4) + 2.0 / 7.0 * pow(z, 5) + 1.0 * pow(z, 6));
                        result += _a_fp[7]() * (1.0 / 23.0 * pow(z, 0) + 71.0 / 161.0 * pow(z, 1) + 681.0 / 3059.0 * pow(z, 2) + 53065.0 / 52003.0 * pow(z, 3) + 1135.0 / 3059.0 * pow(z, 4) + 213.0 / 161.0 * pow(z, 5) + 7.0 / 23.0 * pow(z, 6) + 1.0 * pow(z, 7));
                        result += _a_fp[8]() * (9.0 / 25.0 * pow(z, 0) + 88.0 / 575.0 * pow(z, 1) + 3692.0 / 4025.0 * pow(z, 2) + 5448.0 / 15295.0 * pow(z, 3) + 21226.0 / 15295.0 * pow(z, 4) + 1816.0 / 4025.0 * pow(z, 5) + 852.0 / 575.0 * pow(z, 6) + 8.0 / 25.0 * pow(z, 7) + 1.0 * pow(z, 8));
                        result += _a_fp[9]() * (1.0 / 27.0 * pow(z, 0) + 251.0 / 675.0 * pow(z, 1) + 1076.0 / 5175.0 * pow(z, 2) + 580.0 / 621.0 * pow(z, 3) + 24046.0 / 58995.0 * pow(z, 4) + 290.0 / 207.0 * pow(z, 5) + 7532.0 / 15525.0 * pow(z, 6) + 1004.0 / 675.0 * pow(z, 7) + 1.0 / 3.0 * pow(z, 8) + 1.0 * pow(z, 9));

                        break;
                }

                return result;
            }

            virtual double f_p(const double & q2) const
            {
                const auto z = _z(q2);

                const auto phi      = this->phi_p(z);
                const auto blaschke = this->blaschke_p(q2);
                const auto series   = this->series_p(z);
                double correction_to_asymptotics;
                switch (Process_::asymptotic_case_switch)
                {
                    case 0: // The asymptotics used by Buck and Lebed
                        correction_to_asymptotics = 1.0;
                        break;
                    case 1: // correct asyptotic behaviour
                        correction_to_asymptotics = pow(1.0 - z, 5.0 / 2.0 - 1.0 / 2.0);
                        break;
                    default:
                        correction_to_asymptotics = 1.0;
                        break;
                }
                const auto K        = (5 * M_PI) / 64;

                return 1.0 / (phi * blaschke) * correction_to_asymptotics * sqrt(K) * series;
                // return 1.0 / (phi * blaschke) /* asymptotics*/ * sqrt(K) * series;
            }

            /* f_T */
            virtual double f_t(const double & q2) const
            {
                return 1.0;
            }

            /* f_0 */
            virtual double f_0(const double & /*q2*/) const
            {
                return 0.0; // vanishes exactly
            }
    };

    template <typename Process_> class EGJvD2020FormFactors :
        public EGJvD2020FormFactorBase<Process_, typename Process_::Transition, Process_::has_scalar_form_factor>
    {
        public:
            EGJvD2020FormFactors(const Parameters & p, const Options & o) :
                EGJvD2020FormFactorBase<Process_, typename Process_::Transition, Process_::has_scalar_form_factor>(p, o)
            {
            }

            ~EGJvD2020FormFactors() = default;

            static FormFactors<typename Process_::Transition> * make(const Parameters & parameters, const Options & options)
            {
                return new EGJvD2020FormFactors(parameters, options);
            }
    };
}

#endif
