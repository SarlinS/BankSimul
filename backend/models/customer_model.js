const db = require('../database');


const customer = {
    getById: function(id, callback) {
      return db.query('select * from customer where id_customer = ?', [id], callback);
    },


    getAll: function(callback) {
      return db.query('select * from customer', callback);
    },


    add: function(card, callback) {
      return db.query(
        'insert into customer (fname, lname, street_address, phone_number) values(?,?,?,?)',
        [customer.fname, customer.lname, customer.street_address, customer.phone_number],
        callback
      );
    },


    delete: function(id, callback) {
      return db.query('delete from customer where id_customer = ?', [id], callback);
    },

    
    update: function(id, card, callback) {
      return db.query(
        'update customer set phone_number = ? where id_customer = ?',
        [customer.phone_number, id],
        callback
      );
    }
  };
  module.exports = customer;