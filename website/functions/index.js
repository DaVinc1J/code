require("dotenv").config();

const functions = require("firebase-functions");
const stripe = require("stripe")(process.env.STRIPE_SECRET_KEY);

exports.createStripeCheckout = functions.https.onCall(async (data, context) => {
  try {
    if (!data.quantity || !data.unit_amount || !data.product_name) {
      throw new functions.https.HttpsError(
          "invalid-argument",
          "Missing required parameters.",
      );
    }

    const session = await stripe.checkout.sessions.create({
      payment_method_types: ["card"],
      mode: "payment",
      success_url: "https://hoopify-c1710.web.app/success.html",
      cancel_url: "https://hoopify-c1710.web.app/cancel.html",
      line_items: [
        {
          quantity: data.quantity,
          price_data: {
            currency: "usd",
            unit_amount: data.unit_amount,
            product_data: {
              name: data.product_name,
            },
          },
        },
      ],
    });

    return {id: session.id};
  } catch (error) {
    console.error("Error creating Stripe Checkout session:", error.message);
    throw new functions.https.HttpsError("internal", error.message);
  }
});
