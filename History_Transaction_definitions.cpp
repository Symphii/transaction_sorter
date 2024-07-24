#include <cassert>
#include <iomanip>
#include <iostream>

#include "project4.hpp"
#include "History.hpp"
#include "Transaction.hpp"

////////////////////////////////////////////////////////////////////////////////
// Definitions for Transaction class
////////////////////////////////////////////////////////////////////////////////
//
//

// Constructor
Transaction::Transaction( std::string ticker_symbol,  unsigned int day_date, unsigned int month_date,  
unsigned year_date, bool buy_sell_trans,  unsigned int number_shares,  double trans_amount ) {
  symbol = ticker_symbol;
  day = day_date;
  month = month_date;
  year = year_date;
  if (buy_sell_trans == true) {
    trans_type = "Buy";
  } else {
    trans_type = "Sell";
  }
  shares = number_shares;
  amount = trans_amount;
  trans_id = assigned_trans_id;
  assigned_trans_id++;
  acb = 0.0;
  acb_per_share = 0.0;
  share_balance = 0;
  cgl = 0;
  return;
}

// Destructor
Transaction::~Transaction() {
}

// Overloaded < operator.
bool Transaction::operator<( Transaction const &other ) {
  if (year < other.year) {
    return true;
  } 
  if (year == other.year) {
    if (month < other.month) {
    return true;
    }
    if (month == other.month) {
      if (day < other.day) {
        return true;
      }
      if (day == other.day) {
        if (trans_id < other.trans_id) {
          return true;
        } else {
          return false;
        }
      }
    }
  }
  return false;
}

// GIVEN
// Member functions to get values.
//
std::string Transaction::get_symbol() const { return symbol; }
unsigned int Transaction::get_day() const { return day; }
unsigned int Transaction::get_month() const { return month; }
unsigned int Transaction::get_year() const { return year; }
unsigned int Transaction::get_shares() const { return shares; }
double Transaction::get_amount() const { return amount; }
double Transaction::get_acb() const { return acb; }
double Transaction::get_acb_per_share() const { return acb_per_share; }
unsigned int Transaction::get_share_balance() const { return share_balance; }
double Transaction::get_cgl() const { return cgl; }
bool Transaction::get_trans_type() const { return (trans_type == "Buy") ? true: false ; }
unsigned int Transaction::get_trans_id() const { return trans_id; }
Transaction *Transaction::get_next() { return p_next; }

// GIVEN
// Member functions to set values.
//
void Transaction::set_acb( double acb_value ) { acb = acb_value; }
void Transaction::set_acb_per_share( double acb_share_value ) { acb_per_share = acb_share_value; }
void Transaction::set_share_balance( unsigned int bal ) { share_balance = bal ; }
void Transaction::set_cgl( double value ) { cgl = value; }
void Transaction::set_next( Transaction *p_new_next ) { p_next = p_new_next; }

// GIVEN
// Print the transaction.
//
void Transaction::print() {
  std::cout << std::fixed << std::setprecision(2);
  std::cout << std::setw(4) << get_trans_id() << " "
    << std::setw(4) << get_symbol() << " "
    << std::setw(4) << get_day() << " "
    << std::setw(4) << get_month() << " "
    << std::setw(4) << get_year() << " ";


  if ( get_trans_type() ) {
    std::cout << "  Buy  ";
  } else { std::cout << "  Sell "; }

  std::cout << std::setw(4) << get_shares() << " "
    << std::setw(10) << get_amount() << " "
    << std::setw(10) << get_acb() << " " << std::setw(4) << get_share_balance() << " "
    << std::setw(10) << std::setprecision(3) << get_acb_per_share() << " "
    << std::setw(10) << std::setprecision(3) << get_cgl()
    << std::endl;
}


////////////////////////////////////////////////////////////////////////////////
// Definitions for the History class
////////////////////////////////////////////////////////////////////////////////
//
//


// Constructor
History::History() {
  p_head = nullptr;
  return;
}

// Destructor
History::~History() {
  while ( p_head != nullptr ) {
    Transaction *p_current{ p_head };
    p_head = p_head -> get_next();
    delete p_current;
    p_current = nullptr;
  }
}

// insert(...): Insert transaction into linked list.
void History::insert(Transaction *p_new_trans) {
  if (p_head == nullptr) {
    p_new_trans->set_next( nullptr );
    p_head = p_new_trans;
    return;
  }
  Transaction *p_tail{p_head};
  while (p_tail->get_next() != nullptr) {
    p_tail = p_tail->get_next();
    }
  p_tail->set_next(p_new_trans);
  p_new_trans->set_next( nullptr );
  return;
}
  

// read_history(...): Read the transaction history from file.
void History::read_history() {
  ece150::open_file();
  while (ece150::next_trans_entry() == true) {
    Transaction *current = new Transaction( ece150::get_trans_symbol(),  ece150::get_trans_day(), ece150::get_trans_month(),  
ece150::get_trans_year(), ece150::get_trans_type(),  ece150::get_trans_shares(),  ece150::get_trans_amount() );
    Transaction *p_trans = current;
    History::insert( p_trans );
  }
  ece150::close_file();
  return;
}

// print() Print the transaction history.
void History::print() {
  std::cout << "========== BEGIN TRANSACTION HISTORY ============" << std::endl;
  Transaction *p_dummy = p_head;
  while ( p_dummy != nullptr ) {
    Transaction *p_current{ p_dummy };
    p_dummy = p_dummy->get_next();
    p_current->print();
  }
  std::cout << "========== END TRANSACTION HISTORY ============" << std::endl;
  return;
}

// sort_by_date(): Sort the linked list by trade date.
void History::sort_by_date() {
  Transaction *p_sorted = nullptr;
  while(p_head != nullptr) {
    Transaction *p_1 = p_head;
    Transaction *p_2 = p_head->get_next();
    Transaction *p_largest = p_head;
    Transaction *p_previous = nullptr;
    if (p_head->get_next() == nullptr) {
      p_largest->set_next(p_sorted);
      return;
    }
    while (p_2 != nullptr) {
      if (*p_largest < *p_2) {
        p_largest = p_2;
        p_previous = p_1;
      }
      p_1 = p_1->get_next();
      p_2 = p_2->get_next();
    }
    if (p_previous == nullptr) {
      p_head = p_head->get_next();
      p_largest->set_next(p_sorted);
      p_sorted = p_largest;
    } else {
      p_previous->set_next(p_largest->get_next());
      p_largest->set_next(p_sorted);
      p_sorted = p_largest;
    }
  }
}

// update_acb_cgl(): Updates the ACB and CGL values.
void History::update_acb_cgl() {
  int share_balance = 0;
  double acb = 0.0;
  double acb_share = 0.0;
  double cgl = 0.0;
  Transaction *p_current = p_head;
  while (p_current != nullptr) {
    if (p_current->get_trans_type() == true) {
      share_balance += p_current->get_shares();
      acb += p_current->get_amount();
      acb_share = (acb / share_balance);
      p_current->set_share_balance(share_balance);
      p_current->set_acb(acb);
      p_current->set_acb_per_share(acb_share);
    } else {
      acb -= (acb_share * p_current->get_shares());
      share_balance -= p_current->get_shares();
      cgl = (p_current->get_amount() - (p_current->get_shares() * acb_share));
      p_current->set_share_balance(share_balance);
      p_current->set_acb(acb);
      p_current->set_acb_per_share(acb_share);
      p_current->set_cgl(cgl);
    }
    p_current = p_current->get_next();
  }

}

// compute_cgl(): )Compute the ACB, and CGL.
double History::compute_cgl(unsigned int year) {
  Transaction *p_current = p_head;
  double cgl = 0.0;
  while (p_current != nullptr) {
    if (p_current->get_year() == year) {
      cgl += p_current->get_cgl();
    }
    p_current = p_current->get_next();
  }
  return cgl;
}

// GIVEN
// get_p_head(): Full access to the linked list.
//
Transaction *History::get_p_head() { return p_head; }
