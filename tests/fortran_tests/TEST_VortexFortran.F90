! GNU General Public License v3.0
!
! This file is part of the GAHM model (https://github.com/adcirc/gahm).
! Copyright (c) 2023 ADCIRC Development Group.
!
! This program is free software: you can redistribute it and/or modify
! it under the terms of the GNU General Public License as published by
! the Free Software Foundation, version 3.
!
! This program is distributed in the hope that it will be useful, but
! WITHOUT ANY WARRANTY; without even the implied warranty of
! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
! General Public License for more details.
!
! You should have received a copy of the GNU General Public License
! along with this program. If not, see <http://www.gnu.org/licenses/>.
!
! Author: Zach Cobell
! Contact: zcobell@thewaterinstitute.org
!
module test_criteria
    implicit none

    real(8), parameter :: tol = 1.0e-6

    contains

    logical function is_equal(a, b)
        implicit none
        real(8), intent(in) :: a, b
        is_equal = abs(a - b) <= tol
    end function is_equal

end module test_criteria

program TEST_VortexFortran
    implicit none
    !...Note that we're calling inside a subroutine.
    ! This is because OOP in fortran is a bit different
    ! than in other languages and the cleanup of the
    ! object is not done at the end of a main program,
    ! only when there is a return from another function/subroutine
    call test_001()
end program TEST_VortexFortran

subroutine test_001()
    use test_criteria, only: is_equal
    use gahm_module
    implicit none

    type(gahm_t)                       :: gahm
    type(date_t)                       :: start_date, current_date
    character(200)                     :: filename
    integer                            :: i, j
    integer(8)                         :: n_pts
    real(8),allocatable                :: x(:), y(:), u(:), v(:), p(:)
    logical                            :: exists
    real(8), allocatable               :: solution(:,:)
    real(8),parameter                  :: solution_vec(49*3) = (/&
            0.195283718d0, 0.770345723d0, 1012.502022216d0,&
            0.222376202d0, 0.857573223d0, 1012.443810574d0,&
            0.252043978d0, 0.950423101d0, 1012.381515450d0,&
            0.284442154d0, 1.049019746d0, 1012.314994933d0,&
            0.319729548d0, 1.153479135d0, 1012.244106459d0,&
            0.358068342d0, 1.263907879d0, 1012.168707070d0,&
            0.399623694d0, 1.380402300d0, 1012.088653662d0,&
            0.444563338d0, 1.503047549d0, 1012.003803237d0,&
            0.493057138d0, 1.631916762d0, 1011.914013151d0,&
            0.545276631d0, 1.767070266d0, 1011.819141364d0,&
            0.601394536d0, 1.908554837d0, 1011.719046683d0,&
            0.661584250d0, 2.056403028d0, 1011.613589016d0,&
            0.726019324d0, 2.210632551d0, 1011.502629618d0,&
            0.746840616d0, 2.239979693d0, 1011.479199271d0,&
            0.768076450d0, 2.269164687d0, 1011.455857596d0,&
            0.789723311d0, 2.298154172d0, 1011.432626386d0,&
            0.811776614d0, 2.326913114d0, 1011.409528579d0,&
            0.834230623d0, 2.355404747d0, 1011.386588308d0,&
            0.857078358d0, 2.383590509d0, 1011.363830960d0,&
            0.880311497d0, 2.411429979d0, 1011.341283232d0,&
            0.903920273d0, 2.438880807d0, 1011.318973197d0,&
            0.927893366d0, 2.465898649d0, 1011.296930363d0,&
            0.952217783d0, 2.492437088d0, 1011.275185748d0,&
            0.976878731d0, 2.518447566d0, 1011.253771952d0,&
            1.001859489d0, 2.543879299d0, 1011.232723234d0,&
            1.010344359d0, 2.540385183d0, 1011.251889842d0,&
            1.019055935d0, 2.537141996d0, 1011.270713730d0,&
            1.027998348d0, 2.534145874d0, 1011.289202836d0,&
            1.037175806d0, 2.531392873d0, 1011.307364966d0,&
            1.046626430d0, 2.528963830d0, 1011.325178327d0,&
            1.056270670d0, 2.526643654d0, 1011.342723580d0,&
            1.066168083d0, 2.524567537d0, 1011.359960117d0,&
            1.076323620d0, 2.522731779d0, 1011.376894874d0,&
            1.086742327d0, 2.521132581d0, 1011.393534681d0,&
            1.097429345d0, 2.519766034d0, 1011.409886267d0,&
            1.108389907d0, 2.518628114d0, 1011.425956263d0,&
            1.119629339d0, 2.517714671d0, 1011.441751206d0,&
            1.168065111d0, 2.603114757d0, 1011.376290720d0,&
            1.218500657d0, 2.691063436d0, 1011.308281392d0,&
            1.271030117d0, 2.781655093d0, 1011.237610137d0,&
            1.325732041d0, 2.874942617d0, 1011.164180436d0,&
            1.382687196d0, 2.970977858d0, 1011.087892722d0,&
            1.441978566d0, 3.069811432d0, 1011.008644316d0,&
            1.503691355d0, 3.171492514d0, 1010.926329371d0,&
            1.567912978d0, 3.276068609d0, 1010.840838817d0,&
            1.634733050d0, 3.383585325d0, 1010.752060305d0,&
            1.704243369d0, 3.494086125d0, 1010.659878164d0,&
            1.776537891d0, 3.607612069d0, 1010.564173356d0,&
            1.851712700d0, 3.724201547d0, 1010.464823439d0 /)

    !...Reshape into n x 3 array
    solution = transpose(reshape(solution_vec, (/3,49/)))

    !...Initialization of GAHM begins

    !...Allocate memory for 1 point where we will validate the solution
    allocate(x(1))
    allocate(y(1))
    allocate(u(1))
    allocate(v(1))
    allocate(p(1))

    !...Known good solution
    x(1) = -90.0
    y(1) = 29.0

    filename = "../tests/test_files/bal122005.dat"

    inquire(file=filename, exist=exists)
    if (.not.exists) then
        write(*,'(A)') "[ERROR]: File does not exist"
        call exit(1)
    end if

    n_pts = size(x)

    call gahm%initialize(filename, n_pts, x, y)
    call start_date%set(2005,8,27)
    !...Initialization of GAHM ends

    !...GAHM Time stepping loop
    j = 0
    do i = 0, 86400, 1800
        current_date = start_date%add(int(i, 8))
        call gahm%get(current_date, n_pts, u, v, p)

        j = j + 1

        !...Check the solution
        if (.not.is_equal(u(1), solution(j,1))) then
            write(*,'(A)') "[ERROR]: U values are not equal"
            write(*,'(A,F0.6)') "[ERROR]: u(1) = ", u(1)
            write(*,'(A,I0,A,F0.6)') "[ERROR]: solution(",j,",1) = ", solution(j,1)
            call exit(1)
        end if

        if (.not.is_equal(v(1), solution(j,2))) then
            write(*,'(A)') "[ERROR]: V values are not equal"
            write(*,'(A,F0.6)') "[ERROR]: v(1) = ", v(1)
            write(*,'(A,I0,A,F0.6)') "[ERROR]: solution(",j,",2) = ", solution(j,2)
            call exit(1)
        end if

        if (.not.is_equal(p(1), solution(j,3))) then
            write(*,'(A)') "[ERROR]: P values are not equal"
            write(*,'(A, F0.6)') "[ERROR]: p(1) = ", p(1)
            write(*,'(A,I0,A,F0.6)') "[ERROR]: solution(",j,",3) = ", solution(j,3)
            call exit(1)
        end if

        !write(*,'(F0.9,A,F0.9,A,F0.9,A)') u(1), ", ", v(1), ", ", p(1), ", "

    end do

end subroutine test_001